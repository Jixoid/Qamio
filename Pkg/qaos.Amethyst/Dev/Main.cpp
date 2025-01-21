#include <iostream>
#include <fstream>

#include "JBasic.hpp"
#include "Error.hpp"
#include "Nucleol.hpp"
#include "Wedling.hpp"
#include "Terminal.cpp"

extern "C" {
  #include <sys/reboot.h>
  #include <linux/reboot.h>
  #include <unistd.h>
  #include <wait.h>
}

#include "Modules.cpp"

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
  
  sync();

  reboot(LINUX_REBOOT_CMD_POWER_OFF);
  exit(1);
}

void Start(char* Path) {
  pid_t Pid = fork();
  
  if (Pid == 0) {
    execve(Path, (char *[]){Path, NULL}, NULL);
    
    _exit(1);
  } else if (Pid > 0)
    waitpid(Pid, NULL, 0);

  else
    perror("fork failed");
}



int main() {

  cNucMng NucMng;

  NucMng.GetNucList();
  NucMng.LoadModules();

  NucMng.Check();
  NucMng.Load();
  NucMng.Start();


  Start("/bin/bash");
  

  NucMng.Stop();
  NucMng.Unload();

  SysCtl_Shutdown();

  return 0;
}
