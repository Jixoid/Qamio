/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#define ef else if
#define el else

#include <iostream>
#include <cmath>
#include <format>
#include <vector>

#include "Basis.h"
#include "Kernel.hh"

#include "HAL/Display.hh"

#include <fcntl.h>
#include <unistd.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <drm/drm_fourcc.h>
#include <sys/mman.h>
#include <string.h>

using namespace std;
using namespace jix;
using namespace hal;

using namespace qaos;

#define Log(X)     kernel::Logger(__func__, X)
#define Log2(X,Y)  kernel::Logger(__func__, X, Y)




struct __scr;


struct __screenSess
{
  __scr *Scr;
  display::screenMode Mode;
  drmModeModeInfo *ModeR;

  bool Valid;

  struct __sBuf
  {
    point Map;
    u64   Size;

    u32   FbId;
    u32   Handle;
    u32   Pitch;

    display::bufstate State = display::bufstate::sReady; // u8

    u8 __pad[3]; // padding
  } *Buf;
  
  u8 BufC;
  u8 ActiveBuf = 0;

  drmModeConnector*  Con;
  drmModeEncoder*    Enc;
  drmModeCrtc*       Crt;
};

struct __scr
{
  // GPU
  int   Handle;

  // SCR
  u32 ScrID;

  __screenSess *Sess;
};

struct __gpu
{
  int    Handle;
  string Dev;
  drmModeRes* Res;
};

vector<__gpu> OpenGPU;
vector<__scr> OpenSCR;



// Global
display::sDriver DRV = {

  #define Self ((__screenSess*)__Self)

  // General
  .Reset = []()
  {
    // Clear last
    for (auto &X: OpenSCR)
      if (X.Sess != Nil)
      {
        auto Y = X.Sess;
        X.Sess = Nil;


        // Stop
        if (DRV.IsConnected(Y))
          DRV.DisConnect(Y);

        drmModeFreeConnector(Y->Con); Y->Con = Nil;

        // Valid
        Y->Valid = false;

        Y->Scr = Nil;
      }


    for (auto &X: OpenGPU)
      if (X.Handle != 0)
      {
        drmModeFreeResources(X.Res);
        close(X.Handle);
      }


    OpenGPU.clear();
    OpenSCR.clear();


    
    // Detect all
    drmDevicePtr A[64];
    int CardC = drmGetDevices2(0, A, 64);

    for (int i = 0; i < CardC; ++i)
      OpenGPU.push_back({
        .Handle = 0,
        .Dev    = string(A[i]->nodes[DRM_NODE_PRIMARY]),
        .Res    = Nil,
      });

    drmFreeDevices(A, CardC);    
    
    Log2("Finded gpu count: " +to_string(OpenGPU.size()), kernel::lDebug);



    for (auto &X: OpenGPU)
    {
      // Open device
      X.Handle = open(X.Dev.c_str(), O_RDWR | __O_CLOEXEC);
      if (X.Handle < 0)
        Log2("Can't open device: " +string(X.Dev), kernel::lPanic);


      // Pop resources
      X.Res = drmModeGetResources(X.Handle);
      if (!X.Res) {
        close(X.Handle);
        Log2("Can't load resource: " +string(X.Dev), kernel::lPanic);
      }


      // Pop connectors
      for (int i = 0; i < X.Res->count_connectors; i++)
        OpenSCR.push_back({
          .Handle = X.Handle,

          .ScrID = X.Res->connectors[i],
          .Sess  = Nil,
        });

      Log("Finded screen ["+string(X.Dev)+"]: " +to_string(X.Res->count_connectors));
    }
    
  },

  .Count = []() -> u32
  {
    return OpenSCR.size();
  },



  // Start/Stop
  .Start = [](u32 Index) -> point
  {
    // Interface
    __screenSess *Ret = new __screenSess();
    Ret->Scr = &OpenSCR[Index];
    Ret->Scr->Sess = Ret;

    Ret->Crt = Nil;
    Ret->Enc = Nil;

    
    Log2("ScrSess: " +to_string((u0)Ret), kernel::lIBeg);


    // Connector
    Ret->Con = drmModeGetConnector(Ret->Scr->Handle, Ret->Scr->ScrID);
    if (!Ret->Con)
      Log2("Connector not started", kernel::lPanic);


    Ret->Valid = true;


    // Return
    return (point)Ret;
  },

  .Stop = [](point __Self)
  {
    if (DRV.IsConnected(Self))
      DRV.DisConnect(Self);


    if (Self->Con != Nil)
      drmModeFreeConnector(Self->Con);


    Self->Scr->Sess = Nil;


    Log2("ScrSess: " +to_string((u0)__Self), kernel::lIEnd);

    delete Self;
  },

  .IsValid = [](point __Self) -> bool
  {
    return true;
  },


  // Modes
  .ModeC = [](point __Self) -> u16
  {
    return (Self->Con)->count_modes;
  },

  .ModeG = [](point __Self, u16 Index) -> display::screenMode
  {
    return {
      .Width   = Self->Con->modes[Index].hdisplay,
      .Height  = Self->Con->modes[Index].vdisplay,
      .Refresh = Self->Con->modes[Index].vrefresh,
    };
  },


  // Connect
  .Connect = [](point __Self, u16 Mode, display::format Format, u8 BufCount) -> bool
  {
    // Encoder
    Self->Enc = drmModeGetEncoder(Self->Scr->Handle, Self->Con->encoder_id);
    if (!Self->Enc)
      Log2("Encoder not started", kernel::lPanic);


    // Crtc
    Self->Crt = drmModeGetCrtc(Self->Scr->Handle, Self->Enc->crtc_id);
    if (!Self->Crt)
      Log2("Crtc not started", kernel::lPanic);

    
    // Mode
    u32 ModeId = Self->Con->modes[Mode].type;

    // Push Info
    Self->ModeR = &Self->Con->modes[Mode];
    Self->Mode.Width   = Self->Con->modes[Mode].hdisplay;
    Self->Mode.Height  = Self->Con->modes[Mode].vdisplay;
    Self->Mode.Refresh = Self->Con->modes[Mode].vrefresh;


    // Buffer
    Self->BufC = BufCount;
    Self->Buf = (decltype(Self->Buf))malloc(sizeof(decltype(Self->Buf)) *BufCount);
    if (Self->Buf == Nil)
      Log2("Out of memory", kernel::lPanic);


    for (int i = 0; i <= BufCount-1; ++i)
    {
      // Create Buffer
      drm_mode_create_dumb CReq = {
        .height = Self->Mode.Height,
        .width  = Self->Mode.Width,
        .bpp = 32,
      };

      if (drmIoctl(Self->Scr->Handle, DRM_IOCTL_MODE_CREATE_DUMB, &CReq) < 0)
        Log2("Failed to connected: create buffer", kernel::lPanic);


      Self->Buf[i].Handle = CReq.handle;
      Self->Buf[i].Pitch  = CReq.pitch;
      Self->Buf[i].Size   = CReq.size;


      // Add Buffer
      if (drmModeAddFB(
        Self->Scr->Handle,
        Self->Mode.Width, Self->Mode.Height,
        24, 32, Self->Buf[i].Pitch,
        Self->Buf[i].Handle, &Self->Buf[i].FbId
      ) < 0)
        Log2("Failed to connected: Add framebuffer", kernel::lPanic);



      // Map
      drm_mode_map_dumb MReq = {
        .handle = Self->Buf[i].Handle
      };

      if (drmIoctl(Self->Scr->Handle, DRM_IOCTL_MODE_MAP_DUMB, &MReq) < 0)
        Log2("Failed to connected: Map buffer", kernel::lPanic);
      

      Self->Buf[i].Map = mmap(
        0, Self->Buf[i].Size, 
        PROT_READ | PROT_WRITE, MAP_SHARED,
        Self->Scr->Handle,
        MReq.offset
      );

      if (Self->Buf[i].Map == MAP_FAILED)
        Log2("Failed to connected: MMap buffer", kernel::lPanic);

      memset(Self->Buf[i].Map, 0x00, Self->Buf[i].Size);
    }

    u32 ConID = Self->Con->connector_id;

    // Crtc
    if (drmModeSetCrtc(Self->Scr->Handle, Self->Crt->crtc_id, Self->Buf[0].FbId, 0, 0, &ConID, 1, &Self->Con->modes[Mode]) < 0)
      Log2("Failed to connected: set crtc", kernel::lPanic);


    // Window
    //Self->Window = Graphic.Window.NewWith(Session->Mode.Width, Session->Mode.Height,
    //  Session->Buf[0].Map,
    //  Session->Buf[1].Map
    //);
    //
    //Graphic.Window.Swap(Session->Window);


    Log("ScrSess: " +to_string((u0)__Self));

    return true;
  },

  .DisConnect = [](point __Self)
  {
    // Free Buffer
    for (int i = 0; i <= Self->BufC-1; ++i)
    {
      drmModeRmFB(Self->Scr->Handle, Self->Buf[i].FbId);
      munmap(Self->Buf[i].Map, Self->Buf[i].Size);
    }


    // Free Screen
    drmModeFreeCrtc(Self->Crt); Self->Crt = Nil;
    drmModeFreeEncoder(Self->Enc); Self->Enc = Nil;


    //// Free Window
    //Self->Window->Dis(Self->Window); Self->Window = Nil;

    Log("ScrSess: " +to_string((u0)__Self));
  },

  .IsConnected = [](point __Self) -> bool
  {
    return (Self->Enc != Nil) && (Self->Crt != Nil);
  },

  .IsDevConnected = [](point __Self) -> bool
  {
    return (Self->Con->connection == DRM_MODE_CONNECTED);
  },


  // Buffer
  .BufferSize = [](point __Self) -> u32
  {
    return Self->Mode.Width *Self->Mode.Height *4;
  },

  .BufferC = [](point __Self) -> u8
  {
    return Self->BufC;
  },

  .BufferG = [](point __Self, u8 Index) -> point
  {
    return Self->Buf[Index].Map;
  },


  .BufferStateGet = [](point __Self, u8 Index) -> display::bufstate
  {
    return Self->Buf[Index].State;
  },

  .BufferStateSet = [](point __Self, u8 Index, display::bufstate State)
  {
    Self->Buf[Index].State = State;
  },

  
  .ActiveBuffer = [](point __Self)
  {
    return Self->ActiveBuf;
  },

  .JumpBuffer = [](point __Self, u8 Index)
  {
    Self->ActiveBuf = Index;

    // Write to screen
    u32 ConID = Self->Con->connector_id;

    if (drmModeSetCrtc(
        Self->Scr->Handle,
        Self->Crt->crtc_id,
        Self->Buf[Index].FbId,
        0, 0, &ConID, 1, Self->ModeR
      ) < 0)
      Log2("Set crtc", kernel::lPanic);
  },

  #undef Self
};




// Nuc Control
display::sHAL *HAL = Nil;


extern "C" kernel::nucStd NucStd
{
  .Ver = kernel::NucVer,

  .Load = []()
  {
    DRV.Reset();
  },

  .Unload = []()
  {
    if (HAL != Nil)
      HAL->DelDriver(&DRV);
  },


  .Pop = []()
  {
    HAL = (decltype(HAL))kernel::NucGet(display::Domain);
    if (HAL == Nil)
    {
      Log2(string(display::Domain) +" is not found", kernel::lWarn);
      Log2("Module self-detach", kernel::lError);

      // Cancel installation
      kernel::NucDetach(Module);

      return;
    }
    
  },

  .Push_Drv = []()
  {
    HAL->RegDriver(&DRV);
  },

};
