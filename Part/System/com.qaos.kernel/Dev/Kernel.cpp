/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#define ef else if
#define el else

#include <iostream>
#include <filesystem>
#include <fstream>
#include <thread>
#include <unordered_map>
#include <sys/reboot.h>
#include <sys/utsname.h>
#include <linux/reboot.h>
#include <unistd.h>
#include <wait.h>
#include <sys/mount.h>
#include <string.h>

#include "Basis.h"
#include "Kernel.hpp"

#include "Modules.hpp"

#include "JConf.h"
#include "Neon.h"

#include "COM/Login.hpp"


using namespace std;
using namespace jix;
namespace fs = std::filesystem;

using namespace qaos;
using namespace com;


#define Log(X)     kernel::Logger(__func__, X)
#define Log2(X,Y)  kernel::Logger(__func__, X, Y)


// Global
cNucMng NucMng;


void SysCtl_Shutdown()
{
  cout << "----- Power Off -----" << endl;
  reboot(LINUX_REBOOT_CMD_POWER_OFF);

  exit(0);
}

void Start(char* Path)
{
  pid_t Pid = fork();
  
  if (Pid == 0) {

    char* __argv[] = {Path, Nil};

    execve(Path, __argv, NULL);
    
    _exit(1);
  } else if (Pid > 0)
    waitpid(Pid, NULL, 0);

  else
    perror("fork failed");
}



namespace qaos::kernel
{
  // Memory
  point alloc(uPtr Size)
  {
    return malloc(Size);
  }

  point rlloc(uPtr Size, point Point)
  {
    return realloc(Point, Size);
  }

  void  dispo(point Point)
  {
    free(Point);
  }



  // Nucleol
  unordered_map<string, point> ComList;

  void  NucReg(const char* Key, point Data)
  {
    ComList[string(Key)] = Data;
  }

  point NucGet(const char* Key)
  {
    auto it = ComList.find(string(Key));

    if (it != ComList.end())
      return it->second;
    
    return Nil;
  }


  void  NucDetach(const char* _Module)
  {
    for (auto &Pkg: NucMng.Nucleols)
      if (Pkg.Package == string(_Module))
      {
        Pkg.Detached = true;
        return;
      }

  }


  // Panic
  void Panic(const char* Msg)
  {
    cout << endl;
    cout << ">>>>> Panic <<<<<" << endl;
    cerr << "Qamio PANIC'ed" << endl;
    cerr << Msg << endl;

    sleep(1);
    SysCtl_Shutdown(); 
  }

}



void LoadKMod(string Pkg, string Mod)
{
  // Interface
  string FPath = "/Product/Moq/" +Pkg+ "/Mod/" +Mod+ ".ko";


  // Open file
  ifstream File(FPath, ios::binary | ios::ate);
  if (! File.is_open())
    Log2("File can't open: " +FPath, kernel::lPanic);
    

  // Size
  size_t Size = File.tellg();
  File.seekg(0);


  // Malloc
  void *MData = malloc(Size);
  if (!MData)
    Log2("Out of memory: " +FPath, kernel::lPanic);


  // Read
  File.read((char*)MData, Size);


  // sistem çağrısını
  int ret = syscall(SYS_init_module, MData, Size, "");
  if (ret != 0)
  {
    free(MData);
    Log2(string(strerror(errno)) +": " +FPath, kernel::lPanic);
  }

  Log("Loaded: Product::" +Pkg+ " -> " +Mod);

  
  free(MData);
}


void lkmLoadModules()
{
  // Interface
  vector<pair<string, string>> Mods;


  // Read Config
  jc_stc Conf = jc_Parse("/Product/Conf/Modules.conf");
  if (!jc_IsStc(Conf))
    Log2("Corrupt config file: /Product/Conf/Modules.conf", kernel::lPanic);

  
  jc_arr CMods = jc_StcGet(Conf, "Mods");
  if (!jc_IsArr(CMods))
    Log2("Corrupt config file: /Product/Conf/Modules.conf -> CMods", kernel::lPanic);
  
  
  for (u32 i = 0; i < jc_ArrC(CMods); i++)
  {
    jc_obj Cac;
    char *CStr;

    jc_stc Item = jc_ArrGet(CMods, i);
    if (!jc_IsStc(Item))
      Log2("Corrupt config file: /Product/Conf/Modules.conf -> CMods["+to_string(i)+"]", kernel::lPanic);


    // .Pkg
    Cac = jc_StcGet(Item, "Pkg");
    if (!jc_IsVal(Cac))
      Log2("Corrupt config file: /Product/Conf/Modules.conf -> CMods["+to_string(i)+"].Pkg", kernel::lPanic);
    
    CStr = jc_ValGet(Cac);
    string Pkg = string(CStr);
    jc_DisStr(CStr);
    
    
    // .Mod
    Cac = jc_StcGet(Item, "Mod");
    if (!jc_IsVal(Cac))
      Log2("Corrupt config file: /Product/Conf/Modules.conf -> CMods["+to_string(i)+"].Mod", kernel::lPanic);
    
    CStr = jc_ValGet(Cac);
    string Mod = string(CStr);
    jc_DisStr(CStr);


    Mods.push_back({ Pkg, Mod });
  }


  jc_DisStc(Conf);



  // Load Modules
  for (auto X: Mods)
    LoadKMod(X.first, X.second);
  
}


void MountFS()
{
  struct sMount
  {
    string FS, Path, File, Data;
    u64 Flag;
  };

  // Interface
  vector<sMount> Mounts;


  // Read Config
  jc_stc Conf = jc_Parse("/System/Conf/Mount.conf");
  if (!jc_IsStc(Conf))
    Log2("Corrupt config file: /System/Conf/Mount.conf", kernel::lPanic);


  jc_arr CMounts = jc_StcGet(Conf, "Mounts");
  if (!jc_IsArr(CMounts))
    Log2("Corrupt config file: /System/Conf/Mount.conf -> Mounts", kernel::lPanic);
    

  for (u32 i = 0; i < jc_ArrC(CMounts); i++)
  {
    jc_obj Cac;
    char *CStr;

    jc_stc Item = jc_ArrGet(CMounts, i);
    if (!jc_IsStc(Item))
      Log2("Corrupt config file: /System/Conf/Mount.conf -> Mounts["+to_string(i)+"]", kernel::lPanic);


    sMount M;

    // .FS
    Cac = jc_StcGet(Item, "FS");
    if (!jc_IsVal(Cac))
      Log2("Corrupt config file: /System/Conf/Mount.conf -> Mounts["+to_string(i)+"].FS", kernel::lPanic);
    
    CStr = jc_ValGet(Cac);
    M.FS = string(CStr);
    jc_DisStr(CStr);


    // .Path
    Cac = jc_StcGet(Item, "Path");
    if (!jc_IsVal(Cac))
      Log2("Corrupt config file: /System/Conf/Mount.conf -> Mounts["+to_string(i)+"].Path", kernel::lPanic);
    
    CStr = jc_ValGet(Cac);
    M.Path = string(CStr);
    jc_DisStr(CStr);


    // .File
    Cac = jc_StcGet(Item, "File");
    if (!jc_IsVal(Cac))
      Log2("Corrupt config file: /System/Conf/Mount.conf -> Mounts["+to_string(i)+"].File", kernel::lPanic);
    
    CStr = jc_ValGet(Cac);
    M.File = string(CStr);
    jc_DisStr(CStr);


    // .Data
    Cac = jc_StcGet(Item, "Data");
    if (!jc_IsVal(Cac))
      Log2("Corrupt config file: /System/Conf/Mount.conf -> Mounts["+to_string(i)+"].Data", kernel::lPanic);
    
    CStr = jc_ValGet(Cac);
    M.Data = string(CStr);
    jc_DisStr(CStr);


    // .Flag
    Cac = jc_StcGet(Item, "Flag");
    if (!jc_IsVal(Cac))
      Log2("Corrupt config file: /System/Conf/Mount.conf -> Mounts["+to_string(i)+"].Flag", kernel::lPanic);
    
    CStr = jc_ValGet(Cac);
    M.Flag = stoull(string(CStr));
    jc_DisStr(CStr);


    Mounts.push_back(M);
  }

  jc_DisStc(Conf);



  // Load Modules
  for (auto &X: Mounts)
  {
    if (mount(X.File.c_str(), X.Path.c_str(), X.FS.c_str(), X.Flag, X.Data.c_str()) != 0)
      Log2("Mount {File: "+X.File+", Path: "+X.Path+", FS: "+X.FS+"}: " +strerror(errno), kernel::lPanic);

    Log("Mount {File: "+X.File+", Path: "+X.Path+", FS: "+X.FS+"}");
  }
}



#ifdef CONFIG_Kernel_FastNucControl

void __FastNucControl()
{
  vector<string> Nucs, Provides;


  // Get list
  for (string Part: {"System", "Vendor", "Product"})
    for (const auto &Entry: fs::directory_iterator("/" +Part+ "/Moq"))
      if (Entry.is_directory())
      {
        string Pkg = Entry.path().filename().string();
        
        Nucs.push_back("/"+Part+"/Moq/"+Pkg);
      }


  // Read all Provides
  for (string &Pkg: Nucs)
  {
    string FPath = Pkg+"/Package.conf";

    jc_stc Conf = jc_Parse(FPath.c_str());
    if (Conf == Nil)
      Log2(FPath+" not opened", kernel::lPanic);

    if (!jc_IsStc(Conf))
      Log2("Corrupt config file: "+FPath, kernel::lPanic);


    auto Pro = jc_StcGet(Conf, "Provides");
    if (Pro == Nil)
    {
      jc_DisStc(Conf);
      Log2(FPath+" not readed", kernel::lPanic);
    }


    for (u32 i = 0; i < jc_ArrC(Pro); i++)
    {
      char *Obj = jc_ValGet(jc_ArrGet(Pro, i));
      
      Provides.push_back(string(Obj));

      kernel::dispo(Obj);
    }

  }


  // Read one by one Requires
  for (string &Pkg: Nucs)
  {
    string FPath = Pkg+"/Package.conf";

    jc_stc Conf = jc_Parse(FPath.c_str());
    if (Conf == Nil)
    {
      if (Conf != Nil) jc_DisStc(Conf);
      Log2(FPath+" not opened", kernel::lPanic);
    }

    auto Req = jc_StcGet(Conf, "Requires");
    if (Req == Nil)
    {
      jc_DisStc(Conf);
      Log2(FPath+" not readed", kernel::lPanic);
    }


    vector<string> Requires;

    for (u32 i = 0; i < jc_ArrC(Req); i++)
    {
      char *Obj = jc_ValGet(jc_ArrGet(Req, i));
      
      Requires.push_back(string(Obj));

      kernel::dispo(Obj);
    }

    jc_DisStc(Conf);


    // Control
    for (auto &R: Requires)
    {
      for (auto &P: Provides)
        if (R == P)
          goto __Req_Skip;

      Log2("Dependency \""+R+"\" in \""+fs::path(Pkg).filename().string()+"\" could not be resolved", kernel::lPanic);

      __Req_Skip: {}
    }



  }

}

#endif



void Main()
{
  cout << "----- Hello -----" << endl;
  {
    struct utsname Buf;

    if (uname(&Buf) != 0)
      Log2("Kernel information could not be obtained", kernel::lPanic);


    cout << "Linux " << Buf.release << endl;
  }
  cout << endl;



  #ifdef CONFIG_Kernel_FastNucControl

  cout << "----- Fast Nuc Control -----" << endl;
  __FastNucControl();
  cout << endl;

  #endif


  cout << "----- Start -----" << endl;


  // Mount
  MountFS();

  // lkm Module
  lkmLoadModules();



  // Start Nucs
  NucMng.GetNucList();
  
  NucMng.LoadModules();

  NucMng.Check();
  NucMng.Push();
  NucMng.Pop();
  NucMng.Push_Drv();
  NucMng.Load();
  NucMng.Start();


  // Waked up
  cout << "----- Waked Up -----" << endl;

  login::sDriver *Login = (decltype(Login))kernel::NucGet(login::Domain);
  if (Login == Nil)
    Log2("Depency: "+ string(login::Domain) +" is not found", kernel::lPanic);
  
  Login->Start();



  // Shutdown
  cout << "----- Stop -----" << endl;;
  
  NucMng.Stop();
  NucMng.Unload();
  NucMng.UnloadModules();
}


int main()
{
  try {
    Main();
  } 
  catch(exception &e) {
    cerr << "An exception was encountered" << endl;
    cerr << "Error class: " << typeid(e).name() << endl;
    cerr << "Error message: " << e.what() << endl;

    sleep(1);
  }

  SysCtl_Shutdown();
}
