#include <iostream>
#include <vector>

#include "JBasic.hpp"
#include "Screen.hpp"
#include "Graphic.hpp"


extern "C" {
  #include <fcntl.h>
  #include <unistd.h>
  #include <xf86drm.h>
  #include <xf86drmMode.h>
  #include <drm_fourcc.h>
  #include <sys/mman.h>
  #include <string.h>
}

using namespace std;
using namespace jix;



struct sDrmMng {

  public:

  struct __sBuf {
    int32u  FbId;
    int32u  Handle;
    int8u*  Map;
    int32u  Size;
    int32u  Stride;
  };

  struct __sScreen {
    int32u   ID;
    drmModeConnector*  Con;
    drmModeEncoder*    Enc;
    drmModeCrtc*       Crt;
    
    window2  Window;
    __sBuf   Buf[2];

    sMode  Mode;
  };

  struct __sGPU {
    string       Dev;
    int          Handle;
    drmModeRes*  Res;

    vector<__sScreen> Screens;
  };

  vector<__sGPU>  GPUs;


  public:

  #define Card GPUs[GPU]
  #define Scr GPUs[GPU].Screens[SCR]


  void   GPU_Find() {

    for (auto &A: GPUs)
      if (A.Handle != 0) {

        drmModeFreeResources((drmModeRes*)A.Res);
        close(A.Handle);
      }


    GPUs.clear();


    drmDevicePtr A[32];
    int CardC = drmGetDevices2(0, A, 32);

    if (CardC < 0)
      return;

    for (int i = 0; i < CardC; ++i)
      GPUs.push_back({
        .Dev = A[i]->nodes[DRM_NODE_PRIMARY],
        .Handle = 0,
      });


    drmFreeDevices(A, CardC);
  }

  void   GPU_Open(int32u GPU) {

    if (GPU >= GPUs.size()) throw runtime_error("Failed to find GPU device");


    if (Card.Handle != 0)
      throw runtime_error("Failed to open GPU: Already opened");


    Card.Handle = open(Card.Dev.c_str(), O_RDWR | __O_CLOEXEC);
    if (Card.Handle < 0) 
      throw runtime_error("Failed to open GPU: Can't open device file");


    Card.Res = drmModeGetResources(Card.Handle);
    if (!Card.Res) {
      close(Card.Handle);
      throw runtime_error("Failed to open GPU: Can't open resources");
    }


    Card.Screens.clear();
  }

  void   GPU_Close(int32u GPU) {

    if (GPU >= GPUs.size()) throw runtime_error("Failed to find GPU device");

    
    if (Card.Handle == 0)
      throw runtime_error("Failed to close GPU: Not opened");


    Card.Screens.clear();

    drmModeFreeResources(Card.Res); Card.Res = NULL;
    close(Card.Handle); Card.Handle = 0;
  }


  void   Scr_Find(int32u GPU) {

    if (GPU >= GPUs.size()) throw runtime_error("Failed to find GPU device");


    for (auto &AScr: Card.Screens)
      if (AScr.Con != NULL) {

        drmModeFreeConnector(AScr.Con);
      }

    Card.Screens.clear();


    for (int i = 0; i < (Card.Res)->count_connectors; i++) {
      drmModeConnector* Con = drmModeGetConnector(Card.Handle, (Card.Res)->connectors[i]);
      
      Card.Screens.push_back({
        .ID  = Con->connector_id,
        .Con = NULL,
      });
      drmModeFreeConnector(Con);
    }

  }

  void   Scr_Open(int32u GPU, int32u SCR) {

    if (GPU >= GPUs.size()) throw runtime_error("Failed to find GPU device");
    if (SCR >= Card.Screens.size()) throw runtime_error("Failed to find SCR device");


    if (Scr.Con != 0)
      throw runtime_error("Failed to open Scr: Already opened");


    // Connector
    Scr.Con = drmModeGetConnector(Card.Handle, (Card.Res)->connectors[SCR]);
    if (!Scr.Con)
      throw runtime_error("Failed to open Scr: Invalid connector");

    if (Scr.Con->connection != DRM_MODE_CONNECTED) {
      throw runtime_error("Failed to open Scr: Screen not connected");
    }
    
    int32u ConID = Scr.Con->connector_id;


    // Encoder
    Scr.Enc = drmModeGetEncoder(Card.Handle, Scr.Con->encoder_id);
    if (!Scr.Enc)
      throw runtime_error("Failed to open Scr: Invalid encoder");


    // Crtc
    Scr.Crt = drmModeGetCrtc(Card.Handle, Scr.Enc->crtc_id);


    // Mode
    drmModeModeInfo Mode = Scr.Con->modes[0];
    int32u ModeId = Scr.Con->modes[0].type;


    // Push
    Scr.Mode.Width   = Mode.hdisplay;
    Scr.Mode.Height  = Mode.vdisplay;
    Scr.Mode.Refresh = Mode.vrefresh;


    // Buffer
    for (int i = 0; i < 2; ++i) {
      drm_mode_create_dumb CReq = {};
      CReq.width  = Mode.hdisplay;
      CReq.height = Mode.vdisplay;
      CReq.bpp = 32;
      if (drmIoctl(Card.Handle, DRM_IOCTL_MODE_CREATE_DUMB, &CReq) < 0)
        throw runtime_error("Failed to open Scr: Create dumb buffer");


      Scr.Buf[i].Handle = CReq.handle;
      Scr.Buf[i].Stride = CReq.pitch;
      Scr.Buf[i].Size   = CReq.size;


      if (drmModeAddFB(
        Card.Handle,
        Mode.hdisplay, Mode.vdisplay,
        24, 32, Scr.Buf[i].Stride,
        Scr.Buf[i].Handle, &Scr.Buf[i].FbId
      ) < 0)
        throw runtime_error("Failed to open Scr: Framebuffer");


      drm_mode_map_dumb MReq = {};
      MReq.handle = Scr.Buf[i].Handle;

      if (drmIoctl(Card.Handle, DRM_IOCTL_MODE_MAP_DUMB, &MReq) < 0)
        throw runtime_error("Failed to open Scr: Map dumb buffer");
      

      Scr.Buf[i].Map = static_cast<int8u*>(mmap(
        0, Scr.Buf[i].Size, 
        PROT_READ | PROT_WRITE, MAP_SHARED,
        Card.Handle, MReq.offset)
      );
      if (Scr.Buf[i].Map == MAP_FAILED)
        throw runtime_error("Failed to open Scr: Mmap buffer");

      memset(Scr.Buf[i].Map, 0, Scr.Buf[i].Size);
    }
    
    if (drmModeSetCrtc(Card.Handle, Scr.Crt->crtc_id, Scr.Buf[0].FbId, 0, 0, &ConID, 1, &Mode) < 0)
      throw runtime_error("Failed to open Scr: set CRTC");


    // Window
    Scr.Window = Graphic::Window2_New(0, Scr.Mode.Width, Scr.Mode.Height);
    
    // Assign new buffer
    free(Graphic::Surface2_Ass(0, Graphic::Window2_Sur(0, Scr.Window), (point)Scr.Buf[0].Map));
    free(Graphic::Surface2_Ass(0, Graphic::Window2_Buf(0, Scr.Window), (point)Scr.Buf[1].Map));


    Graphic::Window2_Swp(0, Scr.Window);
  }

  void   Scr_Close(int32u GPU, int32u SCR) {

    if (GPU >= GPUs.size()) throw runtime_error("Failed to find GPU device");
    if (SCR >= Card.Screens.size()) throw runtime_error("Failed to find SCR device");


    if (Scr.Con == 0)
      throw runtime_error("Failed to close Scr: Not opened");

    
    // Free Buffer
    drmModeRmFB(Card.Handle, Scr.Buf[0].FbId);
    drmModeRmFB(Card.Handle, Scr.Buf[1].FbId);
    munmap(Scr.Buf[0].Map, Scr.Buf[0].Size);
    munmap(Scr.Buf[1].Map, Scr.Buf[1].Size);


    // Free Window
    Graphic::Surface2_Ass(0, Graphic::Window2_Sur(0, Scr.Window), NULL);
    Graphic::Surface2_Ass(0, Graphic::Window2_Buf(0, Scr.Window), NULL);

    Graphic::Window2_Dis(0, Scr.Window);


    // Free Screen
    drmModeFreeConnector(Scr.Con); Scr.Con = NULL;
    drmModeFreeEncoder(Scr.Enc); Scr.Enc = NULL;
    drmModeFreeCrtc(Scr.Crt); Scr.Crt = NULL;
  }

  void   Scr_Swap(int32u GPU, int32u SCR) {

    // Swap Buffer
    __sBuf Cac = Scr.Buf[0];
    Scr.Buf[0] = Scr.Buf[1];
    Scr.Buf[1] = Cac;


    // Write to screen
    int32u ConID = Scr.Con->connector_id;
    drmModeModeInfo Mode = Scr.Con->modes[0];

    if (drmModeSetCrtc(Card.Handle, Scr.Crt->crtc_id, Scr.Buf[0].FbId, 0, 0, &ConID, 1, &Mode) < 0)
      throw runtime_error("Failed to open Scr: set CRTC");

  }

  int32u Scr_ModeCount(int32u GPU, int32u SCR) {

    if (GPU >= GPUs.size()) throw runtime_error("Failed to find GPU device");
    if (SCR >= Card.Screens.size()) throw runtime_error("Failed to find SCR device");


    if (Scr.Con == 0)
      throw runtime_error("Failed to close Scr: Not opened");


    return (Scr.Con)->count_modes;
  }

  sMode  Scr_ModeGet(int32u GPU, int32u SCR, int32u Index) {

    if (GPU >= GPUs.size()) throw runtime_error("Failed to find GPU device");
    if (SCR >= Card.Screens.size()) throw runtime_error("Failed to find SCR device");


    if (Index >= Scr.Con->count_modes)
      throw runtime_error("Failed to find Mode device");


    if (Scr.Con == 0)
      throw runtime_error("Failed to close Scr: Not opened");

    return {
      .Width   = Scr.Con->modes[Index].hdisplay,
      .Height  = Scr.Con->modes[Index].vdisplay,
      .Refresh = Scr.Con->modes[Index].vrefresh,
    };
  }


  #undef Scr
  #undef Card

};
