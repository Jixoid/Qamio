#include <iostream>
#include <vector>
#include <tuple>

#include "Nucleol.hpp"
#include "Screen.hpp"
#include "Graphic.hpp"

using namespace std;
using namespace jix;


#define ComSet(X) NucCom.Set(#X, (point)X);




// Global
sNucCom NucCom;


#pragma region NucCom
extern "C" bool NucCheck(int16u Ver) {
  return (gnucVer == Ver);
}


extern "C" void NucPush(sNucCom Com) {

  NucCom = Com;
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

  // Gpu
  vector<int32u> Gpus;

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
  struct sScr {
    int32u Gpu;
    int32u Scr;
    sScreen Screen;
  };
  vector<sScr> Scrs;

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



  // Draw
  for (auto &Scr: Scrs) {
    surface2 Sur = Graphic::Window2_Sur(0, Scr.Screen.Window);


    // Draw
    rect ARect = {
      .L = 0, .T = 0,
      .R = 100, .B = 100,
    };
    Graphic::Surface2_RectF(0, Sur, ARect, 0x00FFFFFF);


    // Swap Buffer
    Graphic::Window2_Swp(0, Scr.Screen.Window);
    Screen::Screen_ScrSwap(Scr.Gpu, Scr.Scr);
  }


  //Screen::Screen_ScrClose(0,0);
}

extern "C" void NucStop() {
  
}

#pragma endregion
