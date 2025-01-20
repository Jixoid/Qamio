#include <iostream>
#include <filesystem>

extern "C" {
  #include <sys/stat.h>
  #include <sys/mount.h>
  #include <sys/reboot.h>
  #include <linux/reboot.h>
  #include <unistd.h>
  #include <wait.h>

  int pivot_root(const char *new_root, const char *put_old);
}

#include "Error.hpp"

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

  cerr << "Catched error: " << ErrToStr(Err) << endl;
  cerr << Msg << endl;

  cout << "Qiniter Panic!" << endl;
  cout << "System is stoping" << endl;
  
  reboot(LINUX_REBOOT_CMD_POWER_OFF);
  exit(1);
}



// Local actions
void Start(char* Path) {
  pid_t pid = fork();
  
  if (pid == 0) {
    // Alt süreçte, hizmet başlatılıyor
    execve(Path, (char *[]) {Path, NULL}, NULL);
    
    // execve başarısız olursa
    _exit(1);
  } else if (pid > 0) {
    // Ana süreç, hizmeti izliyor (PID'yi bekliyor)
    waitpid(pid, NULL, 0);
  } else {
    // Fork hatası
    perror("fork failed");
  }
}

err MountNewRoot(string Device) {
  // Mount NewRoot
  if (mount(Device.c_str(), "/new_root", "ext4", 0, "") != 0)
    return err::Unkwn;

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

err MountKernel() {
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
  
  cout << "Kernel Location: /Moq/" << path << ".moq" << endl;

  fclose(file);

  }
  #pragma endregion

  string Path(path);
  free(path);


  if (!filesystem::create_directory("/Pkg/"+Path))
    return err::New;
  
  if (system("/sbin/modprobe fuse") != 0)
    return err::Unkwn;

  // control /proc/modules
  system("ls /dev/fuse");

  if (system(("archivemount /Moq/"+Path+".moq /Pkg/"+Path+" -o readonly,allow_other").c_str()) != 0)
    return err::Unkwn;


  return err::Ok;
};



const string RootDevice = "/dev/sdb";

int main(){
  printf("Qiniter Started!\n");

  err Err;

  
  Check(MountNewRoot(RootDevice))
    escape(Err, "New root device not mounted! [device: "+RootDevice+"]");

  Check(SwitchRoot())
    escape(Err, "Root not switched\n");

  Check(MountKernel())
    escape(Err, "Kernel not mounted\n");



  //Start("/old_root/bin/bash");


  // Escape
  //SysCtl_Shutdown();

  return 0;
}



/* Basic files
mkdir("/proc", 0700);
mount("none", "/proc", "proc", 0, NULL);

mkdir("/sys", 0700);
mount("none", "/sys", "sysfs", 0, NULL);
*/