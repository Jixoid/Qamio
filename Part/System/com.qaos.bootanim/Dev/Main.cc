/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#include <iostream>
#include <vector>

#include "Basis.h"
#include "Kernel.hh"

#include "HAL/Display.hh"
#include "HAL/Graphic.hh"

#include "COM/BootAnim.hh"


using namespace std;
using namespace jix;
using namespace hal;
using namespace com;
using namespace qaos;


#define Log(X)     kernel::Logger(__func__, X)
#define Log2(X,Y)  kernel::Logger(__func__, X, Y)


display::sHAL *Display;
graphic::sHAL *Graphic;



struct __bootAnimSess
{
  display::sess Disp;
  display::screenMode Mode;
  graphic::sess Buf1, Buf2;
  u8 BufIndex;
};

vector<__bootAnimSess> Anims;


/*
struct __bootAnimSess
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
*/


bootanim::sDriver DRV = {

  .Start = []()
  {
    // Start Display
    if (Display->DRV.Count() == 0)
      Log2("No screen found", kernel::lWarn);


    for (u32 i = 0; i < Display->DRV.Count(); i++)
    {
      auto Cac = Display->DRV.Start(i);
      Display->DRV.Connect(Cac, 0, hal::display::format::iXRGB_8888, 2);

      auto Mode0 = Display->DRV.ModeG(Cac, 0);

      Display->DRV.JumpBuffer(Cac, 0);


      Anims.push_back(__bootAnimSess{
        .Disp = Cac,
        .Mode = Mode0,

        .Buf1 = Graphic->DRV.New_With_Mem(0,
          Mode0.Width,
          Mode0.Height,
          Display->DRV.BufferG(Cac, 0)
        ),

        .Buf2 = Graphic->DRV.New_With_Mem(0,
          Mode0.Width,
          Mode0.Height,
          Display->DRV.BufferG(Cac, 1)
        ),

        .BufIndex = 0,
      });
    }


    for (auto &X: Anims)
    {
      Graphic->DRV.Set_Color(X.Buf2, graphic::color(139.0/255.0, 93.0/255.0, 255.0/255.0));
      
      i32 Min = min<i32>(X.Mode.Width, X.Mode.Height);
      
      Graphic->DRV.Set_FontSize(X.Buf2, Min/4);

      graphic::size2_f32 TSize;
      Graphic->DRV.Calc_Text(X.Buf2, "qamio", &TSize);
      Graphic->DRV.Set_Pos(X.Buf2, {((f32)X.Mode.Width -TSize.W)/2, ((f32)X.Mode.Height -TSize.H)/2});
      Graphic->DRV.Draw_Text(X.Buf2, "qamio");
      
      Graphic->DRV.Fill(X.Buf2);

      Display->DRV.JumpBuffer(X.Disp, 1);
    }

  },

  .Stop = []()
  {

    for (auto &X: Anims)
    {
      Graphic->DRV.Dis(X.Buf1);
      Graphic->DRV.Dis(X.Buf2);

      Display->DRV.DisConnect(X.Disp);
      Display->DRV.Stop(X.Disp);
    }

  },

};



// Nuc Control
extern "C" kernel::nucStd NucStd
{
  .Ver   = kernel::NucVer,

  .Push   = []()
  {
    kernel::NucReg(bootanim::Domain, &DRV);
  },

  .Pop    = []()
  {
    Display = (decltype(Display))kernel::NucGet(display::Domain);
    if (Display == Nil)
      Log2("Depency: "+ string(display::Domain) +" is not found", kernel::lPanic);


    Graphic = (decltype(Graphic))kernel::NucGet(graphic::Domain);
    if (Graphic == Nil)
      Log2("Depency: "+ string(graphic::Domain) +" is not found", kernel::lPanic);
  },

};

