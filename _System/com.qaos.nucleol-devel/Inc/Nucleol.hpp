#pragma once

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>

#include "Basis.hpp"


using namespace std;


namespace jix
{
  // Nucleol Specific
  
  // Except
  inline void qexcept(const char* Func, string Msg)
  {
    throw runtime_error("@" Module "::" + string(Func)+ " " + Msg);
  }

  // Log
  using logLevel = enum: u8 {
    lInfo   = 0,
    lIBeg   = 1,
    lIEnd   = 2,
    lDebug  = 3,
    lWarn   = 4,
    lError  = 5,
    lFatal  = 6,
  };

  namespace {
    const string logLevels[] = {
      "INFO",
      "INFO [BEGIN]",
      "INFO [ END ]",
      "DEBUG",
      "WARN",
      "ERROR",
      "FATAL",
    };
  }

  inline void logger(const char* Func, string Msg, logLevel Level = logLevel::lInfo)
  {
    string Cac = logLevels[Level] +" "
      +"{"+to_string(getpid())+","+to_string(gettid())+"} "
      +"@" Module " "
      +"("+Func+") "
      +Msg +"\n";

    _l_Write:{
      int fd = open("/Data/Log/Qamio.log", O_WRONLY | O_APPEND | O_CREAT, 0666);
      if (fd < 0)
        qexcept(__func__, "Log file not opened");


      if (flock(fd, LOCK_EX) != 0)
      {
        close(fd);
        qexcept(__func__, "Log file not opened");
      }


      // Log >> File
      write(fd, Cac.c_str(), Cac.size());
      
      // Log >> StdLog
      clog << Cac;


      flock(fd, LOCK_UN);
      close(fd);
    }
    sync();


    if (Level == logLevel::lFatal)
      qexcept(Func, Cac);
  }



  // Nucleol Struct
  extern const u16 NucVer;


  using eNucMode = enum: u8 {
    NotWork  = 0,
    Working  = 1,
    Kaizen   = 2,
  };

  struct sNucCom
  {
    void  (*Set)(const char* Key, point);
    point (*Get)(const char* Key);
  };


  struct sNucStd
  {
    u16 Ver  = 0;
    
    void (*Push) (sNucCom)  = Nil;
    void (*Pop)  ()         = Nil;

    void (*Load)  ()        = Nil;
    void (*Unload)()        = Nil;

    void (*Start)()         = Nil;
    void (*Stop) ()         = Nil;
  };


  struct qobject
  {
    void (*Dis)(qobject*);
  };

  struct qsession
  {
    void (*Stop)(qsession*);
  };



  // Standards

  #pragma region Graphic

  using  surface2 = qobject*;
  using  window = qobject*;

  struct rect2d
  {
    f32 L,T,W,H;
  };
  
  struct size2d
  {
    f32 W,H;
  };

  struct point2d
  {
    f32 X,Y;
  };

  struct color
  {
    f32 A,R,G,B;

    color() = default;

    explicit color(f32 nR, f32 nG, f32 nB, f32 nA = 1)
    {
      A = nA;
      R = nR;
      G = nG;
      B = nB;
    }
    
    explicit color(u32 Col)
    {
      A = ((Col >> 24) & 0xFF) /255.0f;
      R = ((Col >> 16) & 0xFF) /255.0f;
      G = ((Col >> 8)  & 0xFF) /255.0f;
      B = ((Col >> 0)  & 0xFF) /255.0f;
    }

    operator u32() const
    {
      return 
        (u32(A*255) << 24) |
        (u32(R*255) << 16) |
        (u32(G*255) << 8) |
        (u32(B*255) << 0);
    }

  };



  struct sGraphic
  {
    struct
    {
      surface2 (*New)    (u16 Width, u16 Height);
      surface2 (*NewWith)(u16 Width, u16 Height, point Buffer);

      color    (*Pixel)  (surface2, point2d Point);
      void     (*Clear)  (surface2);

      size2d   (*SizeGet)(surface2);
      void     (*SizeSet)(surface2, size2d Size);

      point    (*BufferGet)(surface2);
      void     (*BufferSet)(surface2, point Buffer);

      void     (*Draw_Sur2)  (surface2, point2d Point, surface2 Src);
      void     (*Draw_Sur2_A)(surface2, point2d Point, surface2 Src, f32 Alpha);
      void     (*Draw_Png)   (surface2, point2d Point, const char* Path);
      void     (*Draw_Png_A) (surface2, point2d Point, const char* Path, f32 Alpha);
      void     (*Draw_Bmp)   (surface2, point2d Point, const char* Path);
      void     (*Draw_Bmp_A) (surface2, point2d Point, const char* Path, f32 Alpha);
      
      void     (*RectF)  (surface2, rect2d Rect, color Color);
      void     (*RectF_R)(surface2, rect2d Rect, color Color, u16 Round);
      void     (*RectS)  (surface2, rect2d Rect, color Color, u16 Width);
      void     (*RectS_R)(surface2, rect2d Rect, color Color, u16 Width, u16 Round);

      size2d   (*TextSize)(surface2, const char* Text, u16 Width);  
      void     (*Text)    (surface2, const char* Text, u16 Width, point2d Point, color Color);
    } Surface2;

    struct
    {
      window   (*New)    (u16 Width, u16 Height);
      window   (*NewWith)(u16 Width, u16 Height, point Buffer1, point Buffer2);
      
      void     (*Swap)   (window);

      surface2 (*Sur)    (window);
      surface2 (*Buf)    (window);
      
      size2d   (*SizeGet)(window);
      void     (*SizeSet)(window, size2d Size);
    } Window;
  }
  extern Graphic;

  #pragma endregion



  #pragma region Input

  using inputSession = qsession*;

  using inputHandlerREL = void (*)(u16 X, u16 Y);
  using inputHandlerWHL = void (*)(u16 W, u16 H);
  using inputHandlerABS = void (*)(u32 X, u32 Y);


  struct sInput
  {
    void  (*Reset)();
    
    u16   (*Count)();

    struct
    {
      inputSession (*Start)(u16 Index);

      bool (*IsValid)(inputSession);

      void (*SetHandlerREL)(inputSession, inputHandlerREL Handler);
      void (*SetHandlerWHL)(inputSession, inputHandlerWHL Handler);
      void (*SetHandlerABS)(inputSession, inputHandlerABS Handler);

    } Input;
  }
  extern Input;
  
  #pragma endregion



  #pragma region Screen

  using  screenSession = qsession*;


  struct screenMode
  {
    u16 Width;
    u16 Height;
    u32 Refresh;
  };


  struct sScreen
  {
    void  (*Reset)();

    u16   (*Count)();

    struct
    {
      screenSession (*Start)(u16 Index);

      bool (*IsValid)(screenSession);

      u16        (*ModeC)  (screenSession);
      screenMode (*ModeGet)(screenSession, u16 Mode);

      bool  (*IsDevConnected)(screenSession);
      bool  (*IsConnected)   (screenSession);
      void  (*Connect)       (screenSession, u16 Mode);
      void  (*DisConnect)    (screenSession);

      window  (*Window)(screenSession);
      void    (*Swap)  (screenSession);
    } Screen;
  }
  extern Screen;

  #pragma endregion



  #pragma region Battery

  using battery_Status = enum: u8
  {
    bsUnknown     = 0,
    bsCharging    = 1,
    bsDisCharging = 2,
    bsNotCharging = 3,
    bsFull        = 4,
  };


  struct battery_Info
  {
    u8 Percent;
    battery_Status Status;
  };


  struct sBattery
  {
    void  (*Reset)();

    u16   (*Count)();
    battery_Info (*Get)(u16 Index);
  }
  extern Battery;

  #pragma endregion



  #pragma region BootAnim

  using bootAnimSession = qsession*;


  struct sBootAnim
  {
    bootAnimSession (*Start)(screenSession ScrSess);
  }
  extern BootAnim;

  #pragma endregion



  #pragma region Login

  using loginSession = qsession*;
  
  using loginSession_Graphic = loginSession;
  using loginSession_Console = loginSession;


  struct sLogin
  {
    loginSession_Graphic (*Start_Graphic)(screenSession ScrSess);
    loginSession_Console (*Start_Console)();


    bool (*PassOk)(loginSession);

    jstring* (*GetUser)(loginSession);
    jstring* (*GetPass)(loginSession);
  }
  extern Login;

  #pragma endregion



  #pragma region Wedling

  using  secon = qobject*;


  using secPerm = enum: u8
  {
    spRead     = 1,   // Read
    spWrite    = 2,   // Write, Modify
    spExecute  = 4,   // Execute
    spDelete   = 8,   // Delete
    spSee      = 16,  // See
    spAuthS    = 32,  // Change Auth
  };


  struct sWedling
  {
    struct
    {
      secon (*New)(idU Owner);
      
      void  (*Def)(secon, u8 Perm);
      bool  (*Let)(secon, idU Client, u8 Perm, bool Append);
      bool  (*Acc)(secon, idU Client, u8 Perm);
    } Secon;
  }
  extern Wedling;

  #pragma endregion



  #pragma region Widget
  
  using  widget  = qobject*;

  using  wVisual = widget; // : widget

  using  wVisual_mPaint = void (*)(wVisual Sender, surface2 Surface);


  using  wPaint  = wVisual;  // : wvisual : widget
  using  wPanel  = wVisual;  // : wvisual : widget
  using  wButton = wVisual;  // : wvisual : widget
  using  wLabel  = wVisual;  // : wvisual : widget


  struct sWidget
  {
    struct
    {
      widget  (*ParentGet)(widget);
      void    (*ParentSet)(widget, widget Parent);

    } Widget;


    struct
    {
      point2d  (*PointGet)(wVisual);
      void     (*PointSet)(wVisual, point2d Point);
      size2d   (*SizeGet) (wVisual);
      void     (*SizeSet) (wVisual, size2d Size);

      jstring* (*TextGet) (wVisual);
      void     (*TextSet) (wVisual, const char* Text);

      surface2 (*Canvas)  (wVisual);
      void     (*Draw)    (wVisual);
      
      void     (*Input_Rel)(wVisual, i16 X, i16 Y);


      wVisual_mPaint  (*mPaintGet)(wVisual);
      void            (*mPaintSet)(wVisual, wVisual_mPaint Method);

    } WVisual;


    struct
    {
      wPaint   (*New)();
    } WPaint;

    struct
    {
      wPanel   (*New)();
    } WPanel;

    struct
    {
      wButton  (*New)();
    } WButton;

    struct
    {
      wLabel  (*New)();
    } WLabel;

  }
  extern Widget;

  #pragma endregion



  #pragma region Window

  using windowSession = qsession*;
  

  struct sWindow
  {
    windowSession (*Start)(screenSession ScrSess);

    wVisual  (*Root)(windowSession WinSess);
  }
  extern Window;

  #pragma endregion



  #pragma region SystemUI

  using systemUISession = qsession*;


  struct sSystemUI
  {
    systemUISession (*Start)(screenSession ScrSess);

  }
  extern SystemUI;

  #pragma endregion



  #pragma region SystemUI/Notify

  struct sSystemUI_Notify
  {
    wPaint (*New)();
  }
  extern SystemUI_Notify;

  #pragma endregion



  #pragma region SystemUI/Status

  using systemUI_Status_Icon = qobject*;


  struct sSystemUI_Status
  {
    wPaint (*New)();

    struct
    {
      systemUI_Status_Icon (*New)(const char* Text, surface2 Icon);

      surface2 (*IconGet)(systemUI_Status_Icon);
      void     (*IconSet)(systemUI_Status_Icon, surface2 Icon);

      jstring* (*TextGet)(systemUI_Status_Icon);
      void     (*TextSet)(systemUI_Status_Icon, const char* Text);
    } Icon;
  }
  extern SystemUI_Status;

  #pragma endregion

}
