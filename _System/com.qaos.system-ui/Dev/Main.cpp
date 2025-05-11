#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include "Basis.hpp"
#include "Nucleol.hpp"

using namespace std;
using namespace jix;


#define PopNuc(X)  X = *(s##X*)NucCom.Get(#X);
#define PushNuc(X) NucCom = Com; NucCom.Set(#X, &X); X =

#define Log(X)     logger(__func__, X)
#define Log2(X,Y)  logger(__func__, X, Y)





struct __SystemUISess: sessBase
{
  cScreenSess *ScrSess;
  cWindowSess *WinSess;

  cWVisual *Root;

  cWVisual *Notify;
  cWVisual *Status;
};



#pragma region Publish

#define Session ((__SystemUISess*)__Session)

void  SystemUI_Stop(sessBase *__Session)
{
  delete Session->Notify;
  delete Session->Status;


  delete Session->Root;
  delete Session->WinSess;
  delete Session->ScrSess;


  Log2("SystemUISess: " +to_string((uPtr)__Session), lIEnd);
  
  delete Session;
}

#undef Session


systemUISess SystemUI_Start(screenSess ScrSess)
{
  // Interface
  __SystemUISess *Ret = new __SystemUISess();
  Ret->Stop = &SystemUI_Stop;

  Log2("SystemUISess: " +to_string((uPtr)Ret), lIBeg);


  // Prepare
  Ret->ScrSess = new cScreenSess(ScrSess);
  Ret->WinSess = new cWindowSess(*Ret->ScrSess);
  
  Ret->Root = new cWVisual(Ret->WinSess->Root());
  
  size2d Size = Ret->Root->Size();



  #pragma region Load Mods

  // Notify
  Ret->Notify = new cWVisual(SystemUI_Notify.New());

  Ret->Notify->Parent(*Ret->Root);
  Ret->Notify->Point({0, 100});
  Ret->Notify->Size({Size.W/2, 40});


  // Status
  Ret->Status = new cWVisual(SystemUI_Status.New());

  Ret->Status->Parent(*Ret->Root);
  Ret->Status->Point({Size.W/2, 100});
  Ret->Status->Size({Size.W/2, 40});

  #pragma endregion

  
  // Design


  
  // Return
  return (systemUISess)Ret;
}


#pragma endregion






// Global
sNucCom NucCom;


#pragma region NucCom

void Push(sNucCom Com)
{
  PushNuc(SystemUI)
  {
    .Start = SystemUI_Start,
  };
}

void Pop()
{
  PopNuc(Window);
  PopNuc(Widget);

  PopNuc(SystemUI_Notify);
  PopNuc(SystemUI_Status);
}


extern "C" sNucStd NucStd
{
  .Ver = NucVer,

  .Push = &Push,
  .Pop  = &Pop,
};

#pragma endregion
