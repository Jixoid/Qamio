#include <iostream>
#include <vector>
#include <tuple>

#include "Nucleol.hpp"
#include "Screen.hpp"
#include "Graphic.hpp"

using namespace std;
using namespace jix;


#define ComSet(X) NucCom.Set(#X, (point)X);




#pragma region Publish

vector<int32u> Gpus;
struct sScr {
  int32u Gpu;
  int32u Scr;
  sScreen Screen;
  surface2 Image;
  int16u Size;
};
vector<sScr> Scrs;


void BootAnim_Start() {

  // Gpu
  Screen::Screen_GpuFind();
  int32u GpuC = Screen::Screen_GpuCount();
  
  if (0 == GpuC) {
    cout << "@BootAnim: Not found a Gpu" << endl;
    return;
  }

  for (int32u i = 0; i < GpuC; i++) {
    Screen::Screen_GpuOpen(i);
    Gpus.push_back(i);
  }


  // Scr
  for (int32u i = 0; i < GpuC; i++) {
    Screen::Screen_ScrFind(i);
    int32u ScrC = Screen::Screen_ScrCount(i);
    
    for (int32u j = 0; j < ScrC; j++) {
      Screen::Screen_ScrOpen(i,j);
      auto Scr = Screen::Screen_ScrGet(i,j);

      Scrs.push_back({
        .Gpu = i,
        .Scr = j,
        .Screen = Scr
      });

      //cout << "Screen[" << i << "," << j << "]: " << Scr.Mode.Width << "x" << Scr.Mode.Height << "@" << Scr.Mode.Refresh << endl;
    }
  }


  // Prepare
  for (auto &Scr: Scrs) {

    if (Scr.Screen.Mode.Width > Scr.Screen.Mode.Height) {
      // Landscape
      Scr.Size = div(Scr.Screen.Mode.Height, 2).quot;
    } else {
      // Portation
      Scr.Size = Scr.Screen.Mode.Height;
    };
    
    Scr.Image = Graphic::Surface2_New(0, Scr.Size, Scr.Size);
    Graphic::Surface2_Draw_bmp(0, Scr.Image, "/Pkg/qaos.BootAnim/Res/Boot_"+to_string(Scr.Size)+".bmp", {0,0});
  }
  

  // Draw
  for (auto &Scr: Scrs) {
    surface2 Sur = Graphic::Window2_Sur(0, Scr.Screen.Window);


    // Back
    Graphic::Surface2_RectF(0, Sur, {
        .L = 0, .T = 0,
        .R = Scr.Screen.Mode.Width,
        .B = Scr.Screen.Mode.Height,
      },
      Graphic::Surface2_Get(0, Scr.Image, {0,0})
    );


    // Image
    Graphic::Surface2_Draw_Sur2(0, Sur, Scr.Image, {
      .X = (Scr.Screen.Mode.Width  -Scr.Size) /2,
      .Y = (Scr.Screen.Mode.Height -Scr.Size) /2,
    });


    // Swap
    Graphic::Window2_Swp(0, Scr.Screen.Window);
    Screen::Screen_ScrSwap(Scr.Gpu, Scr.Scr);
  }

}

void BootAnim_Stop() {

  // Free Screen
  for (auto &Scr: Scrs) {
    
    Graphic::Surface2_Dis(0, Scr.Image);

    Screen::Screen_ScrClose(Scr.Gpu, Scr.Scr);
  }
  Scrs.clear();


  // Free Gpu
  for (auto &Gpu: Gpus) {
    Screen::Screen_GpuClose(Gpu);
  }
  Gpus.clear();

}

void BootAnim_Prog(int16u Max, int16u Val) {

  // Draw
  for (auto &Scr: Scrs) {
    surface2 Sur = Graphic::Window2_Sur(0, Scr.Screen.Window);


    // Back
    Graphic::Surface2_RectF(0, Sur, {
        .L = 0, .T = 0,
        .R = Scr.Screen.Mode.Width,
        .B = Scr.Screen.Mode.Height,
      },
      Graphic::Surface2_Get(0, Scr.Image, {0,0})
    );


    // Image
    Graphic::Surface2_Draw_Sur2(0, Sur, Scr.Image, {
      .X = (Scr.Screen.Mode.Width  -Scr.Size) /2,
      .Y = (Scr.Screen.Mode.Height -Scr.Size) /2,
    });


    // Prog
    int32u BegX = (Scr.Screen.Mode.Height /9 *5), EndX = (Scr.Screen.Mode.Height /9 *7);


    Graphic::Surface2_RectS(0, Sur, {
      .L = BegX,  .T = (Scr.Screen.Mode.Height /3 *2),
      .R = EndX,  .B = (Scr.Screen.Mode.Height /3 *2) +20,
    }, 0xFFFFFFFF, 1);

    Graphic::Surface2_RectF(0, Sur, {
      .L = BegX +2,
        .T = (Scr.Screen.Mode.Height /3 *2) +2,
      .R = BegX +2 +((EndX -BegX -4) *Val /Max),
        .B = (Scr.Screen.Mode.Height /3 *2) +18,
    }, 0xFFFFFFFF);


    // Swap
    Graphic::Window2_Swp(0, Scr.Screen.Window);
    Screen::Screen_ScrSwap(Scr.Gpu, Scr.Scr);
  }

}

#pragma endregion



// Global
sNucCom NucCom;


#pragma region NucCom
extern "C" bool NucCheck(int16u Ver) {
  return (gnucVer == Ver);
}


extern "C" void NucPush(sNucCom Com) {

  NucCom = Com;

  ComSet(BootAnim_Start);
  ComSet(BootAnim_Stop);
  ComSet(BootAnim_Prog);
}

extern "C" void NucPop() {

  Screen::PopNuc(NucCom);
  Graphic::PopNuc(NucCom);
}


extern "C" void NucLoad() {

}

extern "C" void NucUnload() {

}


extern "C" void NucStart() {

}

extern "C" void NucStop() {
  
}

#pragma endregion
