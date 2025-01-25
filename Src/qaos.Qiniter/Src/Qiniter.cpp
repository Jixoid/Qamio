#include <iostream>
#include <filesystem>
#include <vector>

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


void MountMoq(string Pkg) {

  cout << "[ PEND ] Mounting: " << Pkg << endl;


  try {
    if (!filesystem::create_directory("/Pkg/"+Pkg))
      throw runtime_error("Can not create: /Pkg/"+Pkg);

    if (system(("/bin/archivemount /Moq/"+Pkg+".moq /Pkg/"+Pkg+" -o readonly,allow_other").c_str()) != 0)
      throw runtime_error("Can not mount: "+Pkg);
  }
  catch (runtime_error e) {
    term::Up();
    cout << "[ FAIL ] Mounting: " << Pkg << endl;

    throw e;
  }


  term::Up();
  cout << "[  OK  ] Mounting: " << Pkg << endl;
}


// Local actions
string Kernel_Pkg;

void MountNewRoot(string Device) {
  
  if (mount(Device.c_str(), "/new_root", "ext4", 0, "") != 0)
    throw runtime_error("Can not mount /new_root");

}

void LoadLinuxNuc() {
  
  if (system("/sbin/modprobe fuse") != 0)
    throw runtime_error("Can not load module: fuse");

  // control /proc/modules
  system("ls /dev/fuse");


  
  if (system("/sbin/modprobe drm") != 0)
    throw runtime_error("Can not load module: drm");
  
  if (system("/sbin/modprobe drm_kms_helper") != 0)
    throw runtime_error("Can not load module: drm_kms_helper");
  
  if (system("/sbin/modprobe i915") != 0)
    throw runtime_error("Can not load module: i915");

  if (system("/sbin/modprobe amdgpu") != 0)
    throw runtime_error("Can not load module: amdgpu");

  if (system("/sbin/modprobe nouveau") != 0)
    throw runtime_error("Can not load module: nouveau");

  if (system("/sbin/modprobe vmwgfx") != 0)
    throw runtime_error("Can not load module: vmwgfx");

  if (system("/sbin/modprobe virtio_gpu") != 0)
    throw runtime_error("Can not load module: virtio_gpu");

}

void SwitchRoot() {

  if (umount("/dev") != 0)
    throw runtime_error("Can not umount /dev");


  if (mount("/new_root", "/new_root", NULL, MS_BIND, NULL) != 0)
    throw runtime_error("Can not mount /new_root");

  // Change Root
  chdir("/new_root");

  // Pivot Root
  if (pivot_root(".", "old_root") != 0)
    throw runtime_error("Can not pivot root");


  if (umount("/old_root/new_root") != 0)
    throw runtime_error("Can not umount /old_root/new_root");


  if (mount("none","/dev","devtmpfs", 0, NULL) != 0)
    throw runtime_error("Can not mount /dev");

  if (mount("none","/proc","proc", 0, NULL) != 0)
    throw runtime_error("Can not mount /proc");

  if (mount("none","/sys","sysfs", 0, NULL) != 0)
    throw runtime_error("Can not mount /sys");


  if (mount("ramfs", "/Pkg", "ramfs", 0, NULL) != 0)
    throw runtime_error("Can not mount /Pkg");
  
}

void PrepareKernel() {
  char* path;  

  #pragma region  GetPath
  {

  FILE *file = fopen("/Conf/Kernel.go", "r");
  if (file == NULL)
    throw runtime_error("Can not open /Conf/Kernel.go");

  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  rewind(file);


  path = (char*)malloc(file_size +1);
  if (path == NULL) {
    fclose(file);
    throw runtime_error("Can not alloc mem");
  }
  
  fread(path, 1, file_size, file);
  path[file_size] = '\0';

  fclose(file);

  }
  #pragma endregion


  Kernel_Pkg.assign(path);
  free(path);

};

void Resolver(string Pkg) {

  if (filesystem::exists("/Pkg/"+Pkg))
    return;
  

  MountMoq(Pkg);


  mINI::INIFile file("/Pkg/"+Pkg+"/Package.info");

  mINI::INIStructure ini;
  file.read(ini);


  int ListC = stoi(ini["Depency"]["ListC"]);

  for (int i = 1; i <= ListC; i++)
    MountMoq(ini["Depency"]["List_"+to_string(i)]);

}




const string RootDevice = "/dev/sdb";

int main(){
  term::Clear();

  printf("Qiniter Started!\n");

  
  MountNewRoot(RootDevice);

  LoadLinuxNuc();

  SwitchRoot();

  PrepareKernel();



  term::Clear();

  Resolver(Kernel_Pkg);


  string Srg0 = "/Pkg/"+Kernel_Pkg+"/Bin/Main.elf";
  char*  Arg0 = (char*)Srg0.c_str();

  execve(Arg0, (char* []){Arg0, NULL}, (char* []){"LD_LIBRARY_PATH=/Pkg", NULL});
  return 1;
}
