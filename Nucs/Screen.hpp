#pragma once

#include <iostream>
#include <vector>

#include "JBasic.hpp"
#include "Nucleol.hpp"
#include "Graphic.hpp"

using namespace std;

#define ComGet(X,Y) X = (Y)Com.Get(#X);




namespace jix {

  // Global
  struct sMode {
    int32u Width;
    int32u Height;
    int32u Refresh;
  };
  
  struct sScreen {
    int32u   ID;
    window2  Window;
    bool     Active;

    sMode  Mode;
  };
  
  struct sGPU {
    string  Dev;
  };
  

  // Secons Control
  typedef int32u  (*screen_GpuCount)();
  typedef sGPU    (*screen_GpuGet)  (int32u GPU);
  typedef void    (*screen_GpuFind) ();
  typedef void    (*screen_GpuOpen) (int32u GPU);
  typedef void    (*screen_GpuClose)(int32u GPU);
  
  typedef int32u  (*screen_ScrCount)    (int32u GPU);
  typedef sScreen (*screen_ScrGet)      (int32u GPU, int32u SCR);
  typedef void    (*screen_ScrFind)     (int32u GPU);
  typedef void    (*screen_ScrOpen)     (int32u GPU, int32u SCR);
  typedef void    (*screen_ScrClose)    (int32u GPU, int32u SCR);
  typedef void    (*screen_ScrSwap)     (int32u GPU, int32u SCR);
  typedef int32u  (*screen_ScrModeCount)(int32u GPU, int32u SCR);
  typedef sMode   (*screen_ScrModeGet)  (int32u GPU, int32u SCR, int32u Index);
  

  namespace Screen {
    
    screen_GpuCount  Screen_GpuCount;
    screen_GpuGet    Screen_GpuGet;
    screen_GpuFind   Screen_GpuFind;
    screen_GpuOpen   Screen_GpuOpen;
    screen_GpuClose  Screen_GpuClose;

    screen_ScrCount      Screen_ScrCount;
    screen_ScrGet        Screen_ScrGet;
    screen_ScrFind       Screen_ScrFind;
    screen_ScrOpen       Screen_ScrOpen;
    screen_ScrClose      Screen_ScrClose;
    screen_ScrSwap       Screen_ScrSwap;
    screen_ScrModeCount  Screen_ScrModeCount;
    screen_ScrModeGet    Screen_ScrModeGet;


    void PopNuc(sNucCom Com) {
      ComGet(Screen_GpuCount, screen_GpuCount);
      ComGet(Screen_GpuGet,   screen_GpuGet);
      ComGet(Screen_GpuFind,  screen_GpuFind);
      ComGet(Screen_GpuOpen,  screen_GpuOpen);
      ComGet(Screen_GpuClose, screen_GpuClose);

      ComGet(Screen_ScrCount,     screen_ScrCount);
      ComGet(Screen_ScrGet,       screen_ScrGet);
      ComGet(Screen_ScrFind,      screen_ScrFind);
      ComGet(Screen_ScrOpen,      screen_ScrOpen);
      ComGet(Screen_ScrClose,     screen_ScrClose);
      ComGet(Screen_ScrSwap,      screen_ScrSwap);
      ComGet(Screen_ScrModeCount, screen_ScrModeCount);
      ComGet(Screen_ScrModeGet,   screen_ScrModeGet);
    }
  }


}
