#include <iostream>
#include <vector>

#include "Basis.hpp"
#include "Nucleol.hpp"

using namespace std;
using namespace jix;


#define PopNuc(X)  X = *(s##X*)NucCom.Get(#X);
#define PushNuc(X) NucCom = Com; NucCom.Set(#X, &X); X =

#define Log(X)     logger(__func__, X)
#define Log2(X,Y)  logger(__func__, X, Y)


struct __bootAnimSess: sessBase
{
  cScreenSess* ScrSession;

  cSurface2* BootImg;

  u16 W,H;
  cWindow* Window;
};


#pragma region Publish

#define Session ((__bootAnimSess*)__Session)

void bootAnimSess_Stop(sessBase *__Session)
{
  // Free Anim
  delete Session->BootImg;
  delete Session->Window;
  delete Session->ScrSession;

  
  Log2("BootAnimSess: " +to_string((uPtr)__Session) + ", ScrSess: " +to_string((uPtr)Session->ScrSession), lIEnd);

  // Delete
  delete Session;
}

#undef Session



bootAnimSess _Start(screenSess ScrSession)
{
  __bootAnimSess *Ret = new __bootAnimSess();
  Ret->Stop = &bootAnimSess_Stop;

  Log2("BootAnimSess: " +to_string((uPtr)Ret) + ", ScrSess: " +to_string((uPtr)ScrSession), lIBeg);


  // Start
  Ret->ScrSession = new cScreenSess(ScrSession);
  Ret->Window = new cWindow(Ret->ScrSession->Window());


  size2d Size = Ret->Window->Size();
  Ret->W = Size.W;
  Ret->H = Size.H;


  // Prepare
  Ret->BootImg = new cSurface2(720, 720);
  Ret->BootImg->Draw_Bmp({0,0}, "/Vendor/Conf/BootAnim.bmp");



  // Draw
  cSurface2 Sur(Ret->Window->Sur());


  // Back
  Sur.RectF(
    {
      .L = 0, .T = 0,
      .W = f32(Ret->W),
      .H = f32(Ret->H),
    },
    Ret->BootImg->Pixel({0,0})
  );

  
  // Image
  Sur.Draw_Sur2(
    {
      .X = f32(Ret->W -720) /2,
      .Y = f32(Ret->H -720) /2,
    },
    *Ret->BootImg
  );


  // Swap
  Ret->Window->Swap();
  Ret->ScrSession->Swap();


  // Ret
  return (bootAnimSess)Ret;
}


#pragma endregion



// Global
sNucCom NucCom;


#pragma region NucCom

void Push(sNucCom Com)
{
  PushNuc(BootAnim)
  {
    .Start = &_Start,
  };
}

void Pop()
{
  PopNuc(Graphic)
  PopNuc(Screen);
}



extern "C" sNucStd NucStd
{
  .Ver   = NucVer,

  .Push   = &Push,
  .Pop    = &Pop,
};

#pragma endregion
