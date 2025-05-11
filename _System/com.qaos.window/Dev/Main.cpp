#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
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



struct __windowSess: sessBase
{
  cScreenSess *ScrSession;

  cWindow *Window;
  u16 W,H;
  
  cWVisual *Root;

  // Thread
  atomic<bool> Active;
  thread *Thread;

  // Cursor
  point2d Cursor;


  // Fps
  u16 FPS;
  chrono::nanoseconds Duration;
};




inline void Painter(__windowSess *&Self)
{
  // Process
  Self->Root->Draw();


  // Get Surface
  cSurface2 Sur    = Self->Window->Sur();


  // Copy to buffer
  Sur.Draw_Sur2({0,0}, Self->Root->Canvas());
  

  // Draw Cursor
  Sur.RectF(
    {
      .L = Self->Cursor.X,
      .T = Self->Cursor.Y,
      .W = 32, .H = 32
    }, 
    color(1,1,1,1)
  );


  // Swap
  Self->Window->Swap();
  Self->ScrSession->Swap();
}



void Handler(__windowSess *Self, atomic<bool> *__Ctrl)
{
  // Control
  __Ctrl->store(true);


  // Start
  Log2("Thread", lIBeg);


  auto TBeg = chrono::steady_clock::now();
  auto TEnd = TBeg;

  auto FrameT = TBeg;
  u16 FrameC = 0;

  while (Self->Active.load())
  {
    Painter(Self);
    FrameC++;
    TEnd = chrono::steady_clock::now();


    if (TEnd -FrameT >= std::chrono::seconds(1))
    {
      cout << "FPS: " << FrameC << endl;
      FrameC = 0;
      FrameT = TEnd;
    }


    if ( TEnd-TBeg < Self->Duration )
      this_thread::sleep_for(Self->Duration -(TEnd-TBeg));


    TBeg = chrono::steady_clock::now();
  }
  

  Log2("Thread", lIEnd);
}



#pragma region Publish

#define Session ((__windowSess*)__Session)

void   windowSess_Stop (sessBase *__Session)
{
  // Stop
  Session->Active.store(false);
  Session->Thread->join();


  // Prepare
  delete Session->Root;
  delete Session->Window;
  delete Session->ScrSession;


  Log2("WinSess: " +to_string((uPtr)__Session) +", ScrSess: " +to_string((uPtr)Session->ScrSession), lIEnd);

  delete Session;
}


windowSess windowSess_Start(screenSess ScrSession)
{
  __windowSess *Ret = new __windowSess();
  Ret->Stop = &windowSess_Stop;

  Log2("WinSess: " +to_string((uPtr)Ret) +", ScrSess: " +to_string((uPtr)ScrSession), lIBeg);


  // Prepare
  Ret->ScrSession = new cScreenSess(ScrSession);
  Ret->Window = new cWindow(Ret->ScrSession->Window());
  
  size2d Size = Ret->Window->Size();
  Ret->W = Size.W;
  Ret->H = Size.H;


  Ret->Root = new cWPaint();

  Ret->Root->Size(
    {
      .W = f32(Ret->W),
      .H = f32(Ret->H),
    }
  );


  // Calc
  Ret->FPS = 60;
  Ret->Duration = chrono::nanoseconds(1'000'000'000 / Ret->FPS);


  // Start
  atomic<bool> Ctrl; Ctrl.store(false);

  Ret->Active.store(true);
  Ret->Thread = new thread(Handler, Ret, &Ctrl);


  #warning "Fix this (Thread Launch)"
  while (! Ctrl.load()) {}


  return (windowSess)Ret;
}


wVisual  windowSess_Root(windowSess __Session)
{
  return Session->Root->GetOHID();
}

void   windowSess_Input_Rel(windowSess __Session, i16 X, i16 Y)
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
    .Start = &windowSess_Start,

    .Root  = &windowSess_Root,
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
