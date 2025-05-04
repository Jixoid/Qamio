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


struct __bootAnimSession: qsession
{
  screenSession ScrSession;

  surface2 BootImg;

  u16 W,H;
  window Window;
};


#pragma region Publish

#define Session ((__bootAnimSession*)__Session)

void bootAnimSession_Stop(bootAnimSession __Session)
{
  // Free Anim
  Session->BootImg->Dis(Session->BootImg);

  
  Log2("BootAnimSess: " +to_string((uPtr)__Session) + ", ScrSess: " +to_string((uPtr)Session->ScrSession), lIBeg);

  // Delete
  delete Session;
}

#undef Session



bootAnimSession _Start(screenSession ScrSession)
{
  __bootAnimSession *Ret = new __bootAnimSession();
  Ret->Stop = &bootAnimSession_Stop;
  Ret->ScrSession = ScrSession;

  Log2("BootAnimSess: " +to_string((uPtr)Ret) + ", ScrSess: " +to_string((uPtr)ScrSession), lIBeg);


  // Start
  Ret->Window = Screen.Screen.Window(ScrSession);

  size2d Size = Graphic.Window.SizeGet(Ret->Window);
  Ret->W = Size.W;
  Ret->H = Size.H;


  // Prepare
  Ret->BootImg = Graphic.Surface2.New(720, 720);
  Graphic.Surface2.Draw_Bmp(Ret->BootImg, {0,0}, "/Vendor/Conf/BootAnim.bmp");



  // Draw
  surface2 Sur = Graphic.Window.Sur(Ret->Window);


  // Back
  Graphic.Surface2.RectF(Sur, {
      .L = 0, .T = 0,
      .W = f32(Ret->W),
      .H = f32(Ret->H),
    },
    Graphic.Surface2.Pixel(Ret->BootImg, {0,0})
  );

  
  // Image
  Graphic.Surface2.Draw_Sur2(Sur, {
    .X = f32(Ret->W -720) /2,
    .Y = f32(Ret->H -720) /2,
  }, Ret->BootImg);


  // Swap
  Graphic.Window.Swap(Ret->Window);
  Screen.Screen.Swap(ScrSession);


  // Ret
  return (bootAnimSession)Ret;
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
