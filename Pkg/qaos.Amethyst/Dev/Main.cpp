#include <iostream>
#include <fstream>

#include "JBasic.hpp"
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



#pragma region NucCom
unordered_map<string, point> ComList;

void  NucComSet(string Key, point Val) {
  ComList[Key] = Val;
}

point NucComGet(string Key) {
  auto it = ComList.find(Key);

  if (it != ComList.end())
    return it->second;
  
  return NULL;
}

#pragma endregion




void Main() {

  term::Clear();

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
  usleep(300);
  NucMng.Pop();
  usleep(300);
  
  NucMng.Load();
  usleep(300);
  NucMng.Start();
  usleep(300);


  sleep(5);

  
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

    sleep(3);
  }

  SysCtl_Shutdown();
}
