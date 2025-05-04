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





struct form
{
  wButton But;
};


struct __SystemUISession: qsession
{
  screenSession ScrSess;
  windowSession WinSess;

  wVisual Root;
};



#pragma region Publish

#define Session ((__SystemUISession*)__Session)

void  SystemUI_Stop(systemUISession __Session)
{
  Session->WinSess->Stop(Session->WinSess);


  Log2("SystemUISess: " +to_string((uPtr)__Session), lIEnd);
  
  delete Session;
}

#undef Session


systemUISession SystemUI_Start(screenSession ScrSess)
{
  // Interface
  __SystemUISession *Ret = new __SystemUISession();
  Ret->Stop = &SystemUI_Stop;

  Log2("SystemUISess: " +to_string((uPtr)Ret), lIBeg);


  // Prepare
  Ret->ScrSess = ScrSess;
  Ret->WinSess = Window.Start(Ret->ScrSess);
  
  Ret->Root = Window.Root(Ret->WinSess);
  
  size2d Size = Widget.WVisual.SizeGet(Ret->Root);


  // Design


  
  // Return
  return (systemUISession)Ret;
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
}


extern "C" sNucStd NucStd
{
  .Ver = NucVer,

  .Push = &Push,
  .Pop  = &Pop,
};

#pragma endregion
