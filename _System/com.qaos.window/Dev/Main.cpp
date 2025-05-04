#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <unordered_map>
#include <wait.h>

#include "Nucleol.hpp"

using namespace std;
using namespace jix;


#define PopNuc(X)  X = *(s##X*)NucCom.Get(#X);
#define PushNuc(X) NucCom = Com; NucCom.Set(#X, &X); X =

#define Log(X)     logger(__func__, X)
#define Log2(X,Y)  logger(__func__, X, Y)



struct __windowSession: qsession
{
  screenSession ScrSession;
  window Window;
  u16 W,H;
  
  wVisual Root;

  // Thread
  atomic<bool> Active;
  thread *Thread;

  // Cursor
  point2d Cursor;
};


void Handler(__windowSession *Self, atomic<bool> *__Ctrl)
{
  // Control
  __Ctrl->store(true);


  // Start
  Log2("Thread", lIBeg);

  while (Self->Active.load())
  {
    // Get Surface
    surface2 Sur = Graphic.Window.Sur(Self->Window);


    // Root Draw
    //Widget.WVisual.Input_Rel(Self->Root, Self->Cursor.X, Self->Cursor.Y);  
   
    Widget.WVisual.Draw(Self->Root);

    surface2 Canvas = Widget.WVisual.Canvas(Self->Root);

    Graphic.Surface2.Draw_Sur2(Sur, {0,0}, Canvas);

    // Draw Cursor
    Graphic.Surface2.RectF(Sur, {.L = Self->Cursor.X, .T = Self->Cursor.Y, .W = 32, .H = 32}, color(1,1,1,1));


    // Swap
    Graphic.Window.Swap(Self->Window);
    Screen.Screen.Swap(Self->ScrSession);

    usleep(10000);
  }

  Log2("Thread", lIEnd);
}



#pragma region Publish

#define Session ((__windowSession*)__Session)

void   windowSession_Stop (windowSession __Session)
{
  // Stop
  Session->Active.store(false);
  Session->Thread->join();


  // Prepare
  Session->Root->Dis(Session->Root);


  Log2("WinSess: " +to_string((uPtr)__Session) +", ScrSess: " +to_string((uPtr)Session->ScrSession), lIEnd);

  delete Session;
}


windowSession windowSession_Start(screenSession ScrSession)
{
  __windowSession *Ret = new __windowSession();
  Ret->Stop = &windowSession_Stop;
  Ret->ScrSession = ScrSession;

  Log2("WinSess: " +to_string((uPtr)Ret) +", ScrSess: " +to_string((uPtr)ScrSession), lIBeg);


  // Prepare
  Ret->Window = Screen.Screen.Window(ScrSession);
  
  size2d Size = Graphic.Window.SizeGet(Ret->Window);
  Ret->W = Size.W;
  Ret->H = Size.H;


  Ret->Root = Widget.WPaint.New();

  Widget.WVisual.SizeSet(Ret->Root, {
    .W = f32(Ret->W),
    .H = f32(Ret->H),
  });


  // Start
  atomic<bool> Ctrl; Ctrl.store(false);

  Ret->Active.store(true);
  Ret->Thread = new thread(Handler, Ret, &Ctrl);


  #warning "Fix this (Thread Launch)"
  while (! Ctrl.load()) {}


  return (windowSession)Ret;
}


wVisual  windowSession_Root(windowSession __Session)
{
  return Session->Root;
}

void   windowSession_Input_Rel(windowSession __Session, i16 X, i16 Y)
{
  Session->Cursor.X += X *2;
  Session->Cursor.Y += Y *2;
}

#undef Session

#pragma endregion





// Global
sNucCom NucCom;


#pragma region NucCom

void Push(sNucCom Com)
{
  PushNuc(Window)
  {
    .Start = &windowSession_Start,

    .Root  = &windowSession_Root,
  };
}

void Pop()
{
  PopNuc(Graphic)
  PopNuc(Screen);
  PopNuc(Widget);
}



extern "C" sNucStd NucStd
{
  .Ver = NucVer,

  .Push = &Push,
  .Pop  = &Pop,
};

#pragma endregion
