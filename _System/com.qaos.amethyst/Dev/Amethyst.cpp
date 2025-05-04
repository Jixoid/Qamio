#include <iostream>
#include <fstream>
#include <thread>
#include <unordered_map>
#include <sys/reboot.h>
#include <linux/reboot.h>
#include <unistd.h>
#include <wait.h>
#include <sys/mount.h>
#include <string.h>

#include "Basis.hpp"
#include "Nucleol.hpp"

#include "Modules.hpp"

#include "JConf.hpp"


using namespace std;
using namespace jix;


#define PopNuc(X)  X = *(s##X*)NucCom.Get(#X);

#define Log(X)    logger(__func__, X)
#define Log2(X,Y)  logger(__func__, X, Y)




void SysCtl_Shutdown()
{
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



#pragma region NucCom
unordered_map<string, point> ComList;

void  NucComSet(const char* Key, point Val)
{
  ComList[string(Key)] = Val;
}

point NucComGet(const char* Key)
{
  auto it = ComList.find(string(Key));

  if (it != ComList.end())
    return it->second;
  
  return Nil;
}

#pragma endregion


void LoadModule(string Pkg, string Mod)
{
  // Interface
  string FPath = "/Product/Moq/" +Pkg+ "/Mod/" +Mod+ ".ko";


  // Open file
  ifstream File(FPath, ios::binary | ios::ate);
  if (! File.is_open())
    Log2("File can't open: " +FPath, lFatal);
    

  // Size
  size_t Size = File.tellg();
  File.seekg(0);


  // Malloc
  void *MData = malloc(Size);
  if (!MData)
    Log2("Out of memory: " +FPath, lFatal);


  // Read
  File.read((char*)MData, Size);


  // sistem çağrısını
  int ret = syscall(SYS_init_module, MData, Size, "");
  if (ret != 0)
  {
    free(MData);
    Log2(string(strerror(errno)) +": " +FPath, lFatal);
  }

  Log("Loaded: " +FPath);

  
  free(MData);
}

void LoadLinuxModules()
{
  // Interface
  vector<pair<string, string>> Mods;


  // Read Config
  JConf::cStc *Conf = JConf::ParseRaw("/Product/Conf/Modules.conf");

  for (auto &Item: Conf->GetStc()["Mods"]->GetArr())
  {
    string  Pkg = Item->GetStc()["Pkg"]->GetVal();
    string  Mod = Item->GetStc()["Mod"]->GetVal();

    Mods.push_back({ Pkg, Mod });
  }

  delete Conf;



  // Load Modules
  for (auto X: Mods)
    LoadModule(X.first, X.second);
  
}



void Main()
{
  cout << "Amethyst Started!" << endl;


  #pragma region Linux Basis Mount
  #define __Part "Linux Basis Mount"

  // sys
  if (mount("none", "/sys", "sysfs", 0, NULL) == -1)
    Log2("Mount /sys", lFatal);


  // proc
  if (mount("none", "/proc", "proc", 0, NULL) == -1)
    Log2("Mount /proc", lFatal);
    

  #undef __Part
  #pragma endregion
  

  LoadLinuxModules();



  // Assign NucCom
  sNucCom NucCom;
  NucCom.Set = NucComSet;
  NucCom.Get = NucComGet;


  // Start Nucs
  cNucMng NucMng;

  NucMng.GetNucList();
  
  NucMng.LoadModules();

  NucMng.Check();
  NucMng.Push(NucCom);
  NucMng.Pop();
  NucMng.Load();


  vector<pair<screenSession, qsession*>> Sessions;

  
  #ifdef CONFIG_BootAnim_Active

  Log2("BootAnim", lIBeg);

  PopNuc(BootAnim);
  PopNuc(Screen);


  for (u16 i = 0; i < Screen.Count(); i++)
  {
    screenSession Sess = Screen.Screen.Start(i);

    if (! Screen.Screen.IsDevConnected(Sess))
    {
      Sess->Stop(Sess);
      continue;
    }


    Screen.Screen.Connect(Sess, 0);

    Sessions.push_back({ Sess, Nil });
  }


  for (auto &X: Sessions)
    X.second = BootAnim.Start(X.first);


  #endif

  sleep(1);

  NucMng.Start();

  #ifdef CONFIG_BootAnim_Active
  
  for (auto &X: Sessions)
    X.second->Stop(X.second);
    
  for (auto &X: Sessions)
    X.first->Stop(X.first);

  Sessions.clear();

  Log2("BootAnim", lIEnd);

  #endif



  // Login
  bool PassOk;
  string User, Pass;


  //#warning "Auto Login"
  //PassOk = true;
  //User = "alforce";
  //Pass = "070824";

  //goto _l_SkipLogin;

  #pragma region Login

  #ifdef CONFIG_Login_Style_Graphic
  Log2("Login", lIBeg);


  PopNuc(Login);
  PopNuc(Screen);


  for (u16 i = 0; i < Screen.Count(); i++)
  {
    screenSession Sess = Screen.Screen.Start(i);

    if (! Screen.Screen.IsDevConnected(Sess))
    {
      Sess->Stop(Sess);
      continue;
    }


    Screen.Screen.Connect(Sess, 0);

    Sessions.push_back({ Sess, Nil });
  }
  
  for (auto &X: Sessions)
    X.second = Login.Start_Graphic(X.first);


  sleep(5);


  for (auto &X: Sessions)
    X.second->Stop(X.second);
    
  for (auto &X: Sessions)
    X.first->Stop(X.first);

  Sessions.clear();


  Log2("Login", lIEnd);
  #endif
  
  #ifdef CONFIG_Login_Style_Console
  {
  Log2("Login", lIBeg);


  PopNuc(Login);

  loginSession_Console Sess = Login.Start_Console();

  PassOk = Login.PassOk(Sess);
  if (PassOk)
  {
    jstring *Cac;
    
    Cac = Login.GetUser(Sess);
    User = string(Cac->Str);
    Cac->Dis(Cac);

    Cac = Login.GetPass(Sess);
    Pass = string(Cac->Str);
    Cac->Dis(Cac);
  } 

  Sess->Stop(Sess);


  Log2("Login", lIEnd);
  }
  #endif

  #pragma endregion

  //_l_SkipLogin:



  #ifdef CONFIG_Session_Graphic

  Log2("SystemUI", lIBeg);

  PopNuc(SystemUI);
  PopNuc(Screen);


  for (u16 i = 0; i < Screen.Count(); i++)
  {
    screenSession Sess = Screen.Screen.Start(i);

    if (! Screen.Screen.IsDevConnected(Sess))
    {
      Sess->Stop(Sess);
      continue;
    }


    Screen.Screen.Connect(Sess, 0);

    Sessions.push_back({ Sess, Nil });
  }

  for (auto &X: Sessions)
    X.second = SystemUI.Start(X.first);

    
  sleep(5);

  for (auto &X: Sessions)
    X.second->Stop(X.second);
    
  for (auto &X: Sessions)
    X.first->Stop(X.first);

  Sessions.clear();


  Log2("SystemUI", lIEnd);

  #endif
  #ifdef CONFIG_Session_Console
  #endif


  NucMng.Stop();
  NucMng.Unload();
}


int main() {

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
