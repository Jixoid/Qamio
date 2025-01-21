#include <iostream>
#include <filesystem>
#include <vector>

#include "Error.hpp"
#include "Terminal.cpp"

extern "C" {
  #include <sys/stat.h>
  #include <sys/mount.h>
  #include <sys/reboot.h>
  #include <linux/reboot.h>
  #include <unistd.h>
  #include <wait.h>

  #include <archive.h>
  #include <archive_entry.h>

  int pivot_root(const char *new_root, const char *put_old);
}

#include "ini.h"

#define Check(X)  \
    Err = (X);  \
    if (Err)

using namespace std;
using namespace jix;




void SysCtl_Shutdown() {
  reboot(LINUX_REBOOT_CMD_POWER_OFF);

  exit(0);
}

void escape(err Err, string Msg) {
  #ifdef Debug
  if (Err == err::Ok)
    cerr << "Design Error" << endl;
  #endif

  cerr << "Error: " << ErrToStr(Err) << endl;
  cerr << "Msg: " << Msg << endl;

  cout << "Qiniter Panic!" << endl;
  cout << "System is stoping" << endl;
  
  reboot(LINUX_REBOOT_CMD_POWER_OFF);
  exit(1);
}


err MountMoq(string Pkg) {

  cout << "[ PEND ] Mounting: " << Pkg << endl;


  err Err;

  if (!filesystem::create_directory("/Pkg/"+Pkg)) {
    Err = err::New;
    goto _l_Error;
  }

  if (system(("/bin/archivemount /Moq/"+Pkg+".moq /Pkg/"+Pkg+" -o readonly,allow_other").c_str()) != 0) {
    Err = err::Unkwn;
    goto _l_Error;
  }


  term::Up();
  cout << "[  OK  ] Mounting: " << Pkg << endl;

  return err::Ok;


  _l_Error:
    term::Up();
    cout << "[ FAIL ] Mounting: " << Pkg << endl;

    return Err;
}


// Local actions
string Kernel_Pkg;

err MountNewRoot(string Device) {
  // Mount NewRoot
  if (mount(Device.c_str(), "/new_root", "ext4", 0, "") != 0)
    return err::Unkwn;

  return err::Ok;
}

err LoadLinuxNuc() {
  if (system("/sbin/modprobe fuse") != 0)
    return err::Unkwn;

  // control /proc/modules
  system("ls /dev/fuse");


  return err::Ok;
}

err SwitchRoot() {

  if (umount("/dev") != 0)
    return err::UMoun;


  if (mount("/new_root", "/new_root", NULL, MS_BIND, NULL) != 0)
    return err::Moun;

  // Change Root
  if (chdir("/new_root") != 0)
    return err::Unkwn;

  // Pivot Root
  if (pivot_root(".", "old_root") != 0)
    return err::Unkwn;


  if (umount("/old_root/new_root") != 0)
    return err::UMoun;


  if (mount("none","/dev","devtmpfs", 0, NULL) != 0)
    return err::Moun;

  if (mount("none","/proc","proc", 0, NULL) != 0)
    return err::Moun;

  if (mount("none","/sys","sysfs", 0, NULL) != 0)
    return err::Moun;


  if (mount("ramfs", "/Pkg", "ramfs", 0, NULL) != 0)
    return err::Moun;
  

  return err::Ok;
}

err PrepareKernel() {
  char* path;  

  #pragma region  GetPath
  {

  FILE *file = fopen("/Conf/Kernel.go", "r");
  if (file == NULL)
    return err::NF;

  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  rewind(file);


  path = (char*)malloc(file_size +1);
  if (path == NULL) {
    fclose(file);
    return err::Mem;    
  }
  
  fread(path, 1, file_size, file);
  path[file_size] = '\0';

  fclose(file);

  }
  #pragma endregion


  Kernel_Pkg.assign(path);
  free(path);


  return err::Ok;
};

err Resolver(string Pkg) {

  if (filesystem::exists("/Pkg/"+Pkg))
    return err::Ok;
  

  if (err Err = MountMoq(Pkg))
    return Err;


  mINI::INIFile file("/Pkg/"+Pkg+"/Package.info");

  mINI::INIStructure ini;
  file.read(ini);


  int ListC = stoi(ini["Depency"]["ListC"]);

  for (int i = 1; i <= ListC; i++) {
    if (err Err = MountMoq(ini["Depency"]["List_"+to_string(i)]))
      return Err;
  }
  
  return err::Ok;
}




const string RootDevice = "/dev/sdb";

int main(){
  term::Clear();

  printf("Qiniter Started!\n");

  err Err;

  
  Check(MountNewRoot(RootDevice))
    escape(Err, "New root device not mounted! [device: "+RootDevice+"]");

  Check(LoadLinuxNuc())
    escape(Err, "Linux Modules not loaded");

  Check(SwitchRoot())
    escape(Err, "Root not switched");

  Check(PrepareKernel())
    escape(Err, "Kernel not Founded");


  term::Clear();

  Check(Resolver(Kernel_Pkg))
    escape(Err, "Kernel depency not resolved");



  string Srg0 = "/Pkg/"+Kernel_Pkg+"/Bin/Main.elf";
  char*  Arg0 = (char*)Srg0.c_str();

  execve(Arg0, (char* []){Arg0, NULL}, (char* []){"LD_LIBRARY_PATH=/Pkg", NULL});
  return 1;
}
