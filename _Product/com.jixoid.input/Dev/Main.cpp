#define ef else if


#include <iostream>
#include <vector>
#include <atomic>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <thread>

#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>

#include "Basis.hpp"
#include "Nucleol.hpp"


using namespace std;
using namespace jix;
namespace fs = filesystem;


#define PopNuc(X)  X = *(s##X*)NucCom.Get(#X);
#define PushNuc(X) NucCom = Com; NucCom.Set(#X, &X); X =



struct __input;

struct __inputSession: qsession
{
  __input *Input;

  bool Valid;

  int Handle;

  mutex Wait;

  atomic<bool> Active;
  thread *Thread;

  inputHandlerREL H_REL;
  inputHandlerWHL H_WHL;
  inputHandlerABS H_ABS;

  u32 A_X, A_Y;
};

struct __input
{
  string Path;

  __inputSession *Sess;
};

vector<__input> OpenInput;




bool is_device(string Device)
{
  int fd = open(Device.c_str(), O_RDONLY);
  if (fd == -1) 
    return false;

  
  u64 evbit[EV_MAX /64 +1];
  if (ioctl(fd, EVIOCGBIT(0, EV_MAX), evbit) == -1)
    qexcept(__func__, "Unknown error");


  bool Have_Key = evbit[EV_KEY /64] & (1UL << (EV_KEY %64));
  bool Have_Rel = evbit[EV_REL /64] & (1UL << (EV_REL %64));
  bool Have_Abs = evbit[EV_ABS /64] & (1UL << (EV_ABS %64));

  close(fd);


  return (Have_Key || Have_Rel || Have_Abs);
}

void _Reset()
{
  for (auto &X: OpenInput)
    if (X.Sess != Nil)
    {
      auto Y = X.Sess;
      X.Sess = Nil;


      // Stop //

      // Valid
      Y->Valid = false;

      // Thread
      Y->Active.store(false);
      if (Y->Thread->joinable())
        Y->Thread->join();

      // File
      if (Y->Handle != 0)
      {
        close(Y->Handle);
        Y->Handle = 0;
      }

      // Con Device
      Y->Input = Nil;
    }


  for (auto &Entry: fs::directory_iterator("/dev/input"))
    if (Entry.path().string().find("event") != string::npos && is_device(Entry.path().string()))
      OpenInput.push_back({
        .Path = Entry.path().string(),
        .Sess = Nil,
      });

}



u16  _Count()
{
  return OpenInput.size();
}



void Handler(__inputSession *Session) { while (Session->Active.load())
{
  lock_guard Lock(Session->Wait);
  

  input_event Ev;

  ssize_t Bytes = read(Session->Handle, &Ev, sizeof(Ev));
  
  if (Bytes != sizeof(Ev))
    continue;


  // Keypad
  // Not supported yet //
  if (Ev.type == EV_KEY)
    cout << "Device.Keypad " << (uPtr)Session << ", " << Ev.code << " Durum: " << Ev.value << endl;
  

  // Mouse
  ef (Ev.type == EV_REL)
  {
    // Related //
    if (Session->H_REL == Nil)
      continue;


    if (Ev.code == REL_X)
      Session->H_REL(Ev.value, 0);
    
    ef (Ev.code == REL_Y)
      Session->H_REL(0, Ev.value);



    // Whell //
    if (Session->H_REL == Nil)
      continue;


    if (Ev.code == REL_WHEEL)
      Session->H_WHL(Ev.value, 0);

    ef (Ev.code == REL_HWHEEL)
      Session->H_WHL(0, Ev.value);

  }

  // Tablet
  ef (Ev.type == EV_ABS)
  {
    // Absolute //
    if (Session->H_ABS == Nil)
      continue;


    if (Ev.code == ABS_X)
      Session->A_X = Ev.value;
    
    ef (Ev.code == ABS_Y)
      Session->A_Y = Ev.value;


    Session->H_ABS(Session->A_X, Session->A_Y);
  }

}}



#define Session ((__inputSession*)__Session)


void input·Stop(inputSession __Session)
{
  // Stop
  Session->Active.store(false);
  if (Session->Thread->joinable())
    Session->Thread->join();
  
  delete Session->Thread;
  
  
  if (Session->Handle != 0)
    close(Session->Handle);


  Session->Input->Sess = Nil;

  // Dispose
  delete Session;
}

inputSession input·Start(u16 Index)
{
  // Already ?
  if (OpenInput[Index].Sess != Nil)
    qexcept(__func__, "Device is not suitable");



  // Interface
  __inputSession *Ret = new __inputSession();
  Ret->Stop  = &input·Stop;
  Ret->Input = &OpenInput[Index];
  
  Ret->Input->Sess = Ret;

  Ret->H_REL = Nil;
  Ret->H_WHL = Nil;
  Ret->H_ABS = Nil;


  // Start
  Ret->Handle = open(Ret->Input->Path.c_str(), O_RDONLY | O_NONBLOCK);
  if (Ret->Handle == -1)
    qexcept(__func__, "Can't open device: " +Ret->Input->Path);


  Ret->Valid = true;
  
  Ret->Active.store(true);
  Ret->Thread = new thread(Handler, Ret);


  // Return
  return (inputSession)Ret;
}





// Global
sNucCom NucCom;


#pragma region NucCom

void Push(sNucCom Com)
{
  PushNuc(Input)
  {
    .Reset = &_Reset,

    .Count = &_Count,

    .Input = 
    {
      .Start = &input·Start,

      .IsValid = [](inputSession __Session)-> bool
      {
        return Session->Valid;
      },
      
      
      .SetHandlerREL = [](inputSession __Session, inputHandlerREL Handler)
      {
        if (!Session->Valid)
          qexcept(__func__, "Device is not suitable");

        unique_lock Lock(Session->Wait);
        Session->H_REL = Handler;
      },

      .SetHandlerWHL = [](inputSession __Session, inputHandlerWHL Handler)
      {
        if (!Session->Valid)
          qexcept(__func__, "Device is not suitable");

          unique_lock Lock(Session->Wait);
        Session->H_WHL = Handler;
      },

      .SetHandlerABS = [](inputSession __Session, inputHandlerABS Handler)
      {
        if (!Session->Valid)
          qexcept(__func__, "Device is not suitable");

        unique_lock Lock(Session->Wait);
        Session->H_ABS = Handler;
      },
    }
  };
}

void Pop()
{
  PopNuc(Window);
}


void Load()
{
  _Reset();
}



extern "C" sNucStd NucStd
{
  .Ver = NucVer,

  .Push = &Push,
  .Pop  = &Pop,

  .Load = &Load,
};


#pragma endregion
