#include <iostream>

#include "Nucleol.hpp"
#include "Screen.hpp"
#include "Graphic.hpp"

#include "DrmMng.cpp"

using namespace std;
using namespace jix;

#define ComSet(X) NucCom.Set(#X, (point)X);



// Global
sNucCom NucCom;
sDrmMng* DrmMng = NULL;



int32u  Screen_GpuCount()
  {return DrmMng->GPUs.size(); }

sGPU    Screen_GpuGet  (int32u GPU) {
  return { 
    .Dev = DrmMng->GPUs[GPU].Dev,
  }; 
}

void    Screen_GpuFind()
  {DrmMng->GPU_Find(); }

void    Screen_GpuOpen(int32u GPU)
  {DrmMng->GPU_Open(GPU); }

void    Screen_GpuClose(int32u GPU)
  {DrmMng->GPU_Close(GPU); }


int32u  Screen_ScrCount(int32u GPU)
  {return DrmMng->GPUs[GPU].Screens.size(); }

sScreen Screen_ScrGet(int32u GPU, int32u SCR) {
  return {
    .ID = DrmMng->GPUs[GPU].Screens[SCR].ID,
    .Window = DrmMng->GPUs[GPU].Screens[SCR].Window,
    .Mode = DrmMng->GPUs[GPU].Screens[SCR].Mode,
  }; 
}

void    Screen_ScrFind(int32u GPU)
  {DrmMng->Scr_Find(GPU); }

void    Screen_ScrOpen(int32u GPU, int32u SCR)
  {DrmMng->Scr_Open(GPU, SCR); }

void    Screen_ScrClose(int32u GPU, int32u SCR)
  {DrmMng->Scr_Close(GPU, SCR); }

int32u  Screen_ScrModeCount(int32u GPU, int32u SCR)
  {return DrmMng->Scr_ModeCount(GPU, SCR); }

sMode   Screen_ScrModeGet(int32u GPU, int32u SCR, int32u Index)
  {return DrmMng->Scr_ModeGet(GPU, SCR, Index); }





#pragma region NucCom
extern "C" bool NucCheck(int16u Ver) {
  return (gnucVer == Ver);
}


extern "C" void NucPush(sNucCom Com) {

  NucCom = Com;

  ComSet(Screen_GpuCount);
  ComSet(Screen_GpuGet);
  ComSet(Screen_GpuFind);
  ComSet(Screen_GpuOpen);
  ComSet(Screen_GpuClose);

  ComSet(Screen_ScrCount);
  ComSet(Screen_ScrGet);
  ComSet(Screen_ScrFind);
  ComSet(Screen_ScrOpen);
  ComSet(Screen_ScrClose);
  ComSet(Screen_ScrModeCount);
  ComSet(Screen_ScrModeGet);
}

extern "C" void NucPop() {

  Graphic::PopNuc(NucCom);
}


extern "C" void NucLoad() {

  if (DrmMng != NULL)
    throw runtime_error("Nucleol already loaded");

  DrmMng = new sDrmMng();
}

extern "C" void NucUnload() {

  if (DrmMng == NULL)
    throw runtime_error("Nucleol not loaded");

  delete DrmMng;
}


extern "C" void NucStart() {
  
}

extern "C" void NucStop() {
  
}

#pragma endregion
