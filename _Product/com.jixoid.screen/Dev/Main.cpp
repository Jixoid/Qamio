#include <iostream>
#include <vector>

#include "Nucleol.hpp"

#include <fcntl.h>
#include <unistd.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <drm/drm_fourcc.h>
#include <sys/mman.h>
#include <string.h>

using namespace std;
using namespace jix;

#define PopNuc(X)  X = *(s##X*)NucCom.Get(#X);
#define PushNuc(X) NucCom = Com; NucCom.Set(#X, &X); X =

#define Log(X)    logger(__func__, X)
#define Log2(X,Y)  logger(__func__, X, Y)



struct __scr;


struct __screenSession: qsession
{
  __scr *Scr;
  screenMode Mode;

  bool Valid;

  struct __sBuf
  {
    u32   FbId;
    point Map;

    u32   Handle;
    u32   Pitch;
    u64   Size;
  } Buf[2];

  window  Window;

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

  __screenSession *Sess;
};

struct __gpu
{
  int   Handle;
  char* Dev;
  drmModeRes* Res;
};

vector<__gpu> OpenGPU;
vector<__scr> OpenSCR;



bool screen·IsConnected(screenSession __Session);
void screen·DisConnect(screenSession __Session);



void _Reset()
{
#define __Part "Find Devices"

  Log2("", lIBeg);

  // Clear last
  for (auto &X: OpenSCR)
    if (X.Sess != Nil)
    {
      auto Y = X.Sess;
      X.Sess = Nil;


      // Stop //

      if (screen·IsConnected(Y))
        screen·DisConnect(Y);

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
      .Dev = A[i]->nodes[DRM_NODE_PRIMARY],
    });

  drmFreeDevices(A, CardC);



  // Open all
  Log("Finded gpu: " +to_string(OpenGPU.size()));

  for (auto &X: OpenGPU)
  {
    // Open device
    X.Handle = open(X.Dev, O_RDWR | __O_CLOEXEC);
    if (X.Handle < 0)
      Log2("Can't open device: " +string(X.Dev), lFatal);


    // Pop resources
    X.Res = drmModeGetResources(X.Handle);
    if (!X.Res) {
      close(X.Handle);
      Log2("Can't load resource: " +string(X.Dev), lFatal);
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

  Log("Finded screen: " +to_string(OpenSCR.size()));


  Log2("", lIEnd);

#undef __Part
}



u16  _Count()
{
  return OpenSCR.size();
}




#define Session ((__screenSession*)__Session)


void screen·Stop(screenSession __Session)
{
  if (screen·IsConnected(__Session))
    screen·DisConnect(__Session);


  if (Session->Con != Nil)
    drmModeFreeConnector(Session->Con);


  Session->Scr->Sess = Nil;


  Log2("ScrSess: " +to_string((uPtr)__Session), lIEnd);

  delete Session;
};

screenSession screen·Start(u16 Index)
{
  // Already ?
  if (OpenSCR[Index].Sess != Nil)
    Log2("Device is not suitable", lFatal);



  // Interface
  __screenSession *Ret = new __screenSession();
  Ret->Stop = &screen·Stop;
  Ret->Scr  = &OpenSCR[Index];

  Ret->Scr->Sess = Ret;

  Ret->Window = Nil;


  Log2("ScrSess: " +to_string((uPtr)Ret), lIBeg);


  // Connector
  Ret->Con = drmModeGetConnector(Ret->Scr->Handle, Ret->Scr->ScrID);
  if (!Ret->Con)
    Log2("Connector not started", lFatal);


  Ret->Valid = true;


  // Return
  return (screenSession)Ret;
}


u16 screen·ModeC(screenSession __Session)
{
  if (!Session->Valid)
    Log2("Device is not suitable", lFatal);

  return (Session->Con)->count_modes;
}

screenMode screen·ModeGet(screenSession __Session, u16 Mode)
{
  if (!Session->Valid)
    Log2("Device is not suitable", lFatal);

  return {
    .Width   = Session->Con->modes[Mode].hdisplay,
    .Height  = Session->Con->modes[Mode].vdisplay,
    .Refresh = Session->Con->modes[Mode].vrefresh,
  };
}


bool screen·IsDevConnected(screenSession __Session)
{
  if (!Session->Valid)
    Log2("Device is not suitable", lFatal);

  return (Session->Con->connection == DRM_MODE_CONNECTED);
}

bool screen·IsConnected(screenSession __Session)
{
  if (!Session->Valid)
    Log2("Device is not suitable", lFatal);

  return (Session->Window != Nil);
}

void screen·Connect(screenSession __Session, u16 _Mode)
{
  if (!Session->Valid)
    Log2("Device is not suitable", lFatal);

  // Encoder
  Session->Enc = drmModeGetEncoder(Session->Scr->Handle, Session->Con->encoder_id);
  if (!Session->Enc)
    Log2("Encoder not started", lFatal);


  // Crtc
  Session->Crt = drmModeGetCrtc(Session->Scr->Handle, Session->Enc->crtc_id);
  if (!Session->Crt)
    Log2("Crtc not started", lFatal);

  
  // Mode
  drmModeModeInfo Mode = Session->Con->modes[_Mode];
  u32 ModeId = Session->Con->modes[_Mode].type;

  // Push Info
  Session->Mode.Width   = Mode.hdisplay;
  Session->Mode.Height  = Mode.vdisplay;
  Session->Mode.Refresh = Mode.vrefresh;


  // Buffer
  for (int i = 0; i <= 1; ++i)
  {
    // Create Buffer
    drm_mode_create_dumb CReq = {
      .height = Mode.vdisplay,
      .width  = Mode.hdisplay,
      .bpp = 32,
    };

    if (drmIoctl(Session->Scr->Handle, DRM_IOCTL_MODE_CREATE_DUMB, &CReq) < 0)
      Log2("Failed to connected: create buffer", lFatal);


    Session->Buf[i].Handle = CReq.handle;
    Session->Buf[i].Pitch  = CReq.pitch;
    Session->Buf[i].Size   = CReq.size;


    // Add Buffer
    if (drmModeAddFB(
      Session->Scr->Handle,
      Mode.hdisplay, Mode.vdisplay,
      24, 32, Session->Buf[i].Pitch,
      Session->Buf[i].Handle, &Session->Buf[i].FbId
    ) < 0)
      Log2("Failed to connected: Add framebuffer", lFatal);



    // Map
    drm_mode_map_dumb MReq = {
      .handle = Session->Buf[i].Handle
    };

    if (drmIoctl(Session->Scr->Handle, DRM_IOCTL_MODE_MAP_DUMB, &MReq) < 0)
      Log2("Failed to connected: Map buffer", lFatal);
    

    Session->Buf[i].Map = mmap(
      0, Session->Buf[i].Size, 
      PROT_READ | PROT_WRITE, MAP_SHARED,
      Session->Scr->Handle,
      MReq.offset
    );

    if (Session->Buf[i].Map == MAP_FAILED)
      Log2("Failed to connected: MMap buffer", lFatal);

    memset(Session->Buf[i].Map, 0, Session->Buf[i].Size);
  }


  u32 ConID = Session->Con->connector_id;

  // Crtc
  if (drmModeSetCrtc(Session->Scr->Handle, Session->Crt->crtc_id, Session->Buf[0].FbId, 0, 0, &ConID, 1, &Mode) < 0)
    Log2("Failed to connected: set crtc", lFatal);


  // Window
  Session->Window = Graphic.Window.NewWith(Session->Mode.Width, Session->Mode.Height,
    Session->Buf[0].Map,
    Session->Buf[1].Map
  );

  Graphic.Window.Swap(Session->Window);


  Log("ScrSess: " +to_string((uPtr)__Session));
}

void screen·DisConnect(screenSession __Session)
{
  if (!Session->Valid)
    Log2("Device is not suitable", lFatal);

  // Free Buffer
  drmModeRmFB(Session->Scr->Handle, Session->Buf[0].FbId);
  drmModeRmFB(Session->Scr->Handle, Session->Buf[1].FbId);
  munmap(Session->Buf[0].Map, Session->Buf[0].Size);
  munmap(Session->Buf[1].Map, Session->Buf[1].Size);


  // Free Screen
  drmModeFreeCrtc(Session->Crt); Session->Crt = Nil;
  drmModeFreeEncoder(Session->Enc); Session->Enc = Nil;


  // Free Window
  Session->Window->Dis(Session->Window); Session->Window = Nil;

  Log("ScrSess: " +to_string((uPtr)__Session));
}


window screen·Window(screenSession __Session)
{
  if (!Session->Valid)
    Log2("Device is not suitable", lFatal);

  return Session->Window;
}

void screen·Swap(screenSession __Session)
{
  if (!Session->Valid)
    Log2("Device is not suitable", lFatal);


  // Swap Buffer
  __screenSession::__sBuf Cac = Session->Buf[0];
  Session->Buf[0] = Session->Buf[1];
  Session->Buf[1] = Cac;


  // Write to screen
  u32 ConID = Session->Con->connector_id;
  #warning "Fix This (Invalid Mode)"
  drmModeModeInfo Mode = Session->Con->modes[0];

  if (drmModeSetCrtc(
      Session->Scr->Handle,
      Session->Crt->crtc_id,
      Session->Buf[0].FbId,
      0, 0, &ConID, 1, &Mode
    ) < 0)
    Log2("Set crtc", lFatal);

}




// Global
sNucCom NucCom;


#pragma region NucCom

void Push(sNucCom Com)
{
  PushNuc(Screen)
  {
    .Reset = &_Reset,

    .Count = &_Count,

    .Screen = {
      .Start  = &screen·Start,

      .IsValid = [](screenSession __Session)-> bool
      {
        return Session->Valid;
      },

      .ModeC   = &screen·ModeC,
      .ModeGet = &screen·ModeGet,
      
      .IsDevConnected = &screen·IsDevConnected,
      .IsConnected    = &screen·IsConnected,
      .Connect        = &screen·Connect,
      .DisConnect     = &screen·DisConnect,

      .Window = &screen·Window,
      .Swap   = &screen·Swap,
    },
  };
}

void Pop()
{
  PopNuc(Graphic);
}


void Load()
{
  _Reset();
}


extern "C" sNucStd NucStd
{
  .Ver = NucVer,

  .Push = &Push,
  .Pop  = &Pop,

  .Load = &Load,
};

#pragma endregion
