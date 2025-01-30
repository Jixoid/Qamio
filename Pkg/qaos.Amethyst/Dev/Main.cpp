#include <iostream>
#include <fstream>
#include <thread>

#include "JBasic.hpp"
#include "Nucleol.hpp"
#include "Wedling.hpp"
#include "Terminal.cpp"
#include "BootAnim.hpp"
#include "Login.hpp"

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

  //term::Clear();

  #pragma region Prepare

  // Assign NucCom
  sNucCom NucCom;
  NucCom.Set = NucComSet;
  NucCom.Get = NucComGet;


  // Start Nucs
  cNucMng NucMng;

  NucMng.GetNucList();
  NucMng.LoadModules();

  #pragma endregion
  

  NucMng.Check();



  #pragma region Publish

  NucMng.Push(NucCom);

  #ifdef BootAnim_Active
  BootAnim::PopNuc(NucCom);
  #endif

  #ifdef Login_Active
  Login::PopNuc(NucCom);
  #endif

  #pragma endregion


  NucMng.Pop();



  #pragma region Wakeup

  NucMng.Load();

  #ifdef BootAnim_Active
  BootAnim::BootAnim_Start();
  #endif

  NucMng.Start();

  sleep(2);

  #ifdef BootAnim_Active
  BootAnim::BootAnim_Stop();
  #endif

  #pragma endregion



  #ifdef Login_Active
  Login::Login_ScrStart();

  sleep(20);

  Login::Login_ScrStop();
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
