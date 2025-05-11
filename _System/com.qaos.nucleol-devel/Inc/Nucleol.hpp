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


  struct sessBase
  {
    void (*Stop)(sessBase*);
  };



  // Standards


  // Nuc, Native Obj
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



  class cSurface2
  {
  protected:
    surface2 OHID = Nil;
    bool Dis = true; 

  public:

    // C++
    inline explicit cSurface2(surface2 nOHID) noexcept : Dis(false), OHID(nOHID)
    {}

    inline cSurface2(const cSurface2 &Source) noexcept : Dis(false), OHID(Source.OHID)
    {}

    inline cSurface2(u16 Width, u16 Height)
    {
      OHID = Graphic.Surface2.New(Width, Height);
    }

    inline cSurface2(u16 Width, u16 Height, point Buffer)
    {
      OHID = Graphic.Surface2.NewWith(Width, Height, Buffer);
    }

    inline ~cSurface2()
    {
      if (Dis)
        OHID->Dis(OHID);
    }


    cSurface2& operator=(const cSurface2&) = delete; // Atanamaz


  public:
    
    // API
    inline surface2 GetOHID()
    {
      return OHID;
    }

    inline color Pixel(point2d Point)
    {
      return Graphic.Surface2.Pixel(OHID, Point);
    }

    inline void  Clear()
    {
      Graphic.Surface2.Clear(OHID);
    }


    // Prop
    inline size2d Size()
    {
      return Graphic.Surface2.SizeGet(OHID);
    }

    inline void   Size(size2d Val)
    {
      Graphic.Surface2.SizeSet(OHID, Val);
    }

    
    inline point  Buffer()
    {
      return Graphic.Surface2.BufferGet(OHID);
    }

    inline void   Buffer(point Val)
    {
      Graphic.Surface2.BufferSet(OHID, Val);
    }


    // Draw
    inline void   Draw_Sur2(point2d Point, cSurface2 Source)
    {
      Graphic.Surface2.Draw_Sur2(OHID, Point, Source.OHID);
    }

    inline void   Draw_Sur2(point2d Point, cSurface2 Source, f32 Alpha)
    {
      Graphic.Surface2.Draw_Sur2_A(OHID, Point, Source.OHID, Alpha);
    }

    inline void   Draw_Png(point2d Point, string Path)
    {
      Graphic.Surface2.Draw_Png(OHID, Point, Path.c_str());
    }

    inline void   Draw_Png(point2d Point, string Path, f32 Alpha)
    {
      Graphic.Surface2.Draw_Png_A(OHID, Point, Path.c_str(), Alpha);
    }

    inline void   Draw_Bmp(point2d Point, string Path)
    {
      Graphic.Surface2.Draw_Bmp(OHID, Point, Path.c_str());
    }

    inline void   Draw_Bmp(point2d Point, string Path, f32 Alpha)
    {
      Graphic.Surface2.Draw_Bmp_A(OHID, Point, Path.c_str(), Alpha);
    }

    
    // Shape
    inline void   RectF(rect2d Rect, color Color)
    {
      Graphic.Surface2.RectF(OHID, Rect, Color);
    }

    inline void   RectF(rect2d Rect, color Color, u16 Round)
    {
      Graphic.Surface2.RectF_R(OHID, Rect, Color, Round);
    }
    

    inline void   RectS(rect2d Rect, color Color, u16 Width)
    {
      Graphic.Surface2.RectS(OHID, Rect, Color, Width);
    }

    inline void   RectS(rect2d Rect, color Color, u16 Width, u16 Round)
    {
      Graphic.Surface2.RectS_R(OHID, Rect, Color, Width, Round);
    }

    
    // Text
    inline size2d TextSize(string Text, u16 Width)
    {
      return Graphic.Surface2.TextSize(OHID, Text.c_str(), Width);
    }

    inline void   Text(string Text, u16 Width, point2d Point, color Color)
    {
      Graphic.Surface2.Text(OHID, Text.c_str(), Width, Point, Color);
    }

  };

  class cWindow
  {
  protected:
    window OHID = Nil;
    bool Dis = true; 

  public:

    // C++
    inline explicit cWindow(surface2 nOHID) noexcept : Dis(false), OHID(nOHID)
    {}

    inline cWindow(const cWindow &Source) noexcept: Dis(false), OHID(Source.OHID)
    {}

    inline cWindow(u16 Width, u16 Height)
    {
      OHID = Graphic.Window.New(Width, Height);
    }

    inline cWindow(u16 Width, u16 Height, point Buffer1, point Buffer2)
    {
      OHID = Graphic.Window.NewWith(Width, Height, Buffer1, Buffer2);
    }

    inline ~cWindow()
    {
      if (Dis)
        OHID->Dis(OHID);
    }


    cSurface2& operator=(const cSurface2&) = delete; // Atanamaz


  public:

    // API
    inline window GetOHID()
    {
      return OHID;
    }

    inline void  Swap()
    {
      Graphic.Window.Swap(OHID);
    }
    

    // Prop
    inline size2d Size()
    {
      return Graphic.Window.SizeGet(OHID);
    }

    inline void   Size(size2d Val)
    {
      Graphic.Window.SizeSet(OHID, Val);
    }


    inline cSurface2 Sur()
    {
      return cSurface2(Graphic.Window.Sur(OHID));
    }

    inline cSurface2 Buf()
    {
      return cSurface2(Graphic.Window.Buf(OHID));
    }

  };

  #pragma endregion



  // Nuc, Native Obj
  #pragma region Input

  using inputSess = struct: sessBase {}*;
  

  using inputHandlerREL = void (*)(u16 X, u16 Y);
  using inputHandlerWHL = void (*)(u16 W, u16 H);
  using inputHandlerABS = void (*)(u32 X, u32 Y);



  struct sInput
  {
    void  (*Reset)();
    
    u16   (*Count)();

    struct
    {
      inputSess (*Start)(u16 Index);

      bool (*IsValid)(inputSess);

      void (*SetHandlerREL)(inputSess, inputHandlerREL Handler);
      void (*SetHandlerWHL)(inputSess, inputHandlerWHL Handler);
      void (*SetHandlerABS)(inputSess, inputHandlerABS Handler);

    } Input;
  }
  extern Input;
  


  class cInputSess
  {
  protected:
    inputSess OHID = Nil;
    bool Dis = true; 

  public:

    // C++
    inline explicit cInputSess(inputSess nOHID) noexcept : Dis(false), OHID(nOHID)
    {}

    inline cInputSess(const cInputSess &Source) noexcept : Dis(false), OHID(Source.OHID)
    {}

    inline explicit cInputSess(u16 Index)
    {
      OHID = Input.Input.Start(Index);
    }

    inline ~cInputSess()
    {
      if (Dis)
        OHID->Stop(OHID);
    }


    cInputSess& operator=(const cInputSess&) = delete; // Atanamaz

  
  public:
    
    // API
    inline inputSess GetOHID()
    {
      return OHID;
    }


    inline bool IsValid()
    {
      return Input.Input.IsValid(OHID);
    }

    
    inline void HandlerREL(inputHandlerREL Handler)
    {
      Input.Input.SetHandlerREL(OHID, Handler);
    }

    inline void HandlerWHL(inputHandlerWHL Handler)
    {
      Input.Input.SetHandlerWHL(OHID, Handler);
    }

    inline void HandlerABS(inputHandlerABS Handler)
    {
      Input.Input.SetHandlerABS(OHID, Handler);
    }

  };

  #pragma endregion



  // Nuc, Native Obj
  #pragma region Screen

  using screenSess = struct: sessBase {}*;


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
      screenSess (*Start)(u16 Index);

      bool (*IsValid)(screenSess);

      u16        (*ModeC)  (screenSess);
      screenMode (*ModeGet)(screenSess, u16 Mode);

      bool  (*IsDevConnected)(screenSess);
      bool  (*IsConnected)   (screenSess);
      void  (*Connect)       (screenSess, u16 Mode);
      void  (*DisConnect)    (screenSess);

      window  (*Window)(screenSess);
      void    (*Swap)  (screenSess);
    } Screen;
  }
  extern Screen;



  class cScreenSess
  {
  protected:
    screenSess OHID = Nil;
    bool Dis = true; 

  public:

    // C++
    inline explicit cScreenSess(screenSess nOHID) noexcept : Dis(false), OHID(nOHID)
    {}

    inline cScreenSess(const cScreenSess &Source) noexcept : Dis(false), OHID(Source.OHID)
    {}

    inline explicit cScreenSess(u16 Index)
    {
      OHID = Screen.Screen.Start(Index);
    }

    inline ~cScreenSess()
    {
      if (Dis)
        OHID->Stop(OHID);
    }


    cScreenSess& operator=(const cScreenSess&) = delete; // Atanamaz


  public:
    
    // API
    inline screenSess GetOHID()
    {
      return OHID;
    }


    inline bool IsValid()
    {
      return Screen.Screen.IsValid(OHID);
    }


    inline void   Swap()
    {
      Screen.Screen.Swap(OHID);
    }

    inline cWindow Window()
    {
      return cWindow(Screen.Screen.Window(OHID));
    }
    

    inline bool  IsDevConnected()
    {
      return Screen.Screen.IsDevConnected(OHID);
    }

    inline bool  IsConnected()
    {
      return Screen.Screen.IsConnected(OHID);
    }

    inline void  Connect(u16 Mode)
    {
      Screen.Screen.Connect(OHID, Mode);
    }

    inline void  DisConnect()
    {
      Screen.Screen.DisConnect(OHID);
    }


    inline u16        ModeC()
    {
      return Screen.Screen.ModeC(OHID);
    }

    inline screenMode ModeGet(u16 Mode)
    {
      return Screen.Screen.ModeGet(OHID, Mode);
    }

  };

  #pragma endregion



  // Nuc
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



  // Nuc, Native Obj
  #pragma region BootAnim

  using bootAnimSess = struct: sessBase {}*;


  struct sBootAnim
  {
    bootAnimSess (*Start)(screenSess ScrSess);
  }
  extern BootAnim;



  class cBootAnimSess
  {
  protected:
    bootAnimSess OHID = Nil;
    bool Dis = true; 

  public:

    // C++
    inline explicit cBootAnimSess(bootAnimSess nOHID) noexcept : Dis(false), OHID(nOHID)
    {}

    inline cBootAnimSess(const cBootAnimSess &Source) noexcept : Dis(false), OHID(Source.OHID)
    {}

    inline explicit cBootAnimSess(cScreenSess ScreenSess)
    {
      OHID = BootAnim.Start(ScreenSess.GetOHID());
    }

    inline ~cBootAnimSess()
    {
      if (Dis)
        OHID->Stop(OHID);
    }


    cBootAnimSess& operator=(const cBootAnimSess&) = delete; // Atanamaz


  public:
    
    // API
    inline bootAnimSess GetOHID()
    {
      return OHID;
    }


  };

  #pragma endregion



  // Nuc, Native Obj
  #pragma region Login

  using loginSess = struct: sessBase {}*;
  
  using loginSess_Graphic = loginSess;
  using loginSess_Console = loginSess;


  struct sLogin
  {
    loginSess_Graphic (*Start_Graphic)(screenSess ScrSess);
    loginSess_Console (*Start_Console)();


    bool (*PassOk)(loginSess);

    jstring* (*GetUser)(loginSess);
    jstring* (*GetPass)(loginSess);
  }
  extern Login;



  class cLoginSess
  {
  protected:
    loginSess OHID = Nil;
    bool Dis = true; 

  public:

    // C++
    inline explicit cLoginSess(loginSess nOHID) noexcept : Dis(false), OHID(nOHID)
    {}

    inline cLoginSess(const cLoginSess &Source) noexcept : Dis(false), OHID(Source.OHID)
    {}

    inline cLoginSess() noexcept
    {}
    
    inline ~cLoginSess() noexcept
    {}


    cLoginSess& operator=(const cLoginSess&) = delete; // Atanamaz

  
  public:
    
    // API
    inline loginSess GetOHID()
    {
      return OHID;
    }


    inline bool PassOK()
    {
      return Login.PassOk(OHID);
    }

    inline string  GetUser()
    {
      jstring *Cac = Login.GetUser(OHID);

      string Ret(Cac->Str);

      Cac->Dis(Cac);

      return Ret;
    }

    inline string  GetPass()
    {
      jstring *Cac = Login.GetPass(OHID);

      string Ret(Cac->Str);

      Cac->Dis(Cac);

      return Ret;
    }

  };


  class cLoginSess_Graphic: public cLoginSess
  {
  public:

    // C++
    inline explicit cLoginSess_Graphic(loginSess_Graphic nOHID)
    {
      Dis = false;
      OHID = nOHID;
    }

    inline cLoginSess_Graphic(const cLoginSess_Graphic &Source)
    {
      Dis = false;
      OHID = Source.OHID;
    }

    inline explicit cLoginSess_Graphic(screenSess ScrSess)
    {
      OHID = Login.Start_Graphic(ScrSess);
    }

    inline ~cLoginSess_Graphic()
    {
      if(Dis)
        OHID->Stop(OHID);
    }


    cLoginSess_Graphic& operator=(const cLoginSess_Graphic&) = delete; // Atanamaz

  };


  class cLoginSess_Console: public cLoginSess
  {
  public:

    // C++
    inline explicit cLoginSess_Console(loginSess_Console nOHID)
    {
      Dis = false;
      OHID = nOHID;
    }

    inline cLoginSess_Console(const cLoginSess_Console &Source)
    {
      Dis = false;
      OHID = Source.OHID;
    }

    inline cLoginSess_Console()
    {
      OHID = Login.Start_Console();
    }

    inline ~cLoginSess_Console()
    {
      if(Dis)
        OHID->Stop(OHID);
    }


    cLoginSess_Console& operator=(const cLoginSess_Console&) = delete; // Atanamaz

  };

  #pragma endregion



  // Nuc, Native Obj
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


  class cWedling
  {
  protected:
    secon OHID = Nil;
    bool Dis = true; 

  public:

    // C++
    inline explicit cWedling(secon nOHID) noexcept : Dis(false), OHID(nOHID)
    {}

    inline cWedling(const cWedling &Source) noexcept : Dis(false), OHID(Source.OHID)
    {}

    inline explicit cWedling(idU Owner)
    {
      OHID = Wedling.Secon.New(Owner);
    }

    inline ~cWedling()
    {
      if (Dis)
        OHID->Dis(OHID);
    }


    cWedling& operator=(const cWedling&) = delete; // Atanamaz


  public:
    
    // API
    inline secon GetOHID()
    {
      return OHID;
    }


    inline void  Def(u8 Perm)
    {
      Wedling.Secon.Def(OHID, Perm);
    }

    inline bool  Let(idU Client, u8 Perm, bool Append = true)
    {
      return Wedling.Secon.Let(OHID, Client, Perm, Append);
    }

    inline bool  Acc(idU Client, u8 Perm)
    {
      return Wedling.Secon.Acc(OHID, Client, Perm);
    }

  };

  #pragma endregion



  // Nuc, Native Obj
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
      widget   (*New)();

      widget  (*ParentGet)(widget);
      void    (*ParentSet)(widget, widget Parent);

    } Widget;


    struct
    {
      wVisual   (*New)();

      point2d  (*PointGet)(wVisual);
      void     (*PointSet)(wVisual, point2d Point);
      size2d   (*SizeGet) (wVisual);
      void     (*SizeSet) (wVisual, size2d Size);

      jstring* (*TextGet) (wVisual);
      void     (*TextSet) (wVisual, const char* Text);

      surface2 (*Canvas)  (wVisual);
      void     (*Draw)    (wVisual);
      
      void     (*Input_Abs)(wVisual, f32 X, f32 Y);


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



  class cWidget
  {
  protected:
    widget OHID = Nil;
    bool Dis = true; 

  public:

    // C++
    inline explicit cWidget(widget nOHID) noexcept : Dis(false), OHID(nOHID)
    {}

    inline cWidget(const cWidget &Source) noexcept : Dis(false), OHID(Source.OHID)
    {}

    inline cWidget()
    {
      OHID = Widget.Widget.New();
    }

    inline ~cWidget()
    {
      if (Dis)
        OHID->Dis(OHID);
    }


    cWidget& operator=(const cWidget&) = delete; // Atanamaz


  public:
    
    // API
    inline widget GetOHID()
    {
      return OHID;
    }


    // Prop
    inline cWidget  Parent()
    {
      return cWidget(Widget.Widget.ParentGet(OHID));
    }

    inline void     Parent(cWidget Val)
    {
      Widget.Widget.ParentSet(OHID, Val.GetOHID());
    }

  };


  class cWVisual: public cWidget
  {
  public:

    // C++
    inline explicit cWVisual(wVisual nOHID)
    {
      Dis = false;
      OHID = nOHID;
    }

    inline cWVisual(const cWVisual &Source)
    {
      Dis = false;
      OHID = Source.OHID;
    }

    inline cWVisual()
    {
      OHID = Widget.WVisual.New();
    }

    inline ~cWVisual()
    {
      if (Dis)
        OHID->Dis(OHID);
    }


    cWVisual& operator=(const cWVisual&) = delete; // Atanamaz


  public:
    
    // API
    inline void  Draw()
    {
      Widget.WVisual.Draw(OHID);
    }


    // Prop
    inline point2d Point()
    {
      return Widget.WVisual.PointGet(OHID);
    }

    inline void    Point(point2d Val)
    {
      Widget.WVisual.PointSet(OHID, Val);
    }


    inline size2d  Size()
    {
      return Widget.WVisual.SizeGet(OHID);
    }

    inline void    Size(size2d Val)
    {
      Widget.WVisual.SizeSet(OHID, Val);
    }


    inline string  Text()
    {
      jstring *Cac = Widget.WVisual.TextGet(OHID);

      string Ret(Cac->Str);

      Cac->Dis(Cac);

      return Ret;
    }
    
    inline void    Text(string Val)
    {
      Widget.WVisual.TextSet(OHID, Val.c_str());
    }


    inline cSurface2 Canvas()
    {
      return cSurface2(Widget.WVisual.Canvas(OHID));
    }


    // Prom m(s)
    inline wVisual_mPaint  mPaint()
    {
      return Widget.WVisual.mPaintGet(OHID);
    }

    inline void            mPaint(wVisual_mPaint Val)
    {
      Widget.WVisual.mPaintSet(OHID, Val);
    }


    // Method
    inline void  Input_Abs(f32 X, f32 Y)
    {
      Widget.WVisual.Input_Abs(OHID, X,Y);
    }

  };




  class cWPaint: public cWVisual
  {
  public:

    // C++
    inline explicit cWPaint(wPaint nOHID)
    {
      Dis = false;
      OHID = nOHID;
    }

    inline cWPaint(const cWPaint &Source)
    {
      Dis = false;
      OHID = Source.OHID;
    }

    inline cWPaint()
    {
      OHID = Widget.WPaint.New();
    }

    inline ~cWPaint()
    {
      if (Dis)
        OHID->Dis(OHID);
    }


    cWPaint& operator=(const cWPaint&) = delete; // Atanamaz

  };


  class cWPanel: public cWVisual
  {
  public:

    // C++
    inline explicit cWPanel(wPanel nOHID)
    {
      Dis = false;
      OHID = nOHID;
    }

    inline cWPanel(const cWPanel &Source)
    {
      Dis = false;
      OHID = Source.OHID;
    }

    inline cWPanel()
    {
      OHID = Widget.WPanel.New();
    }

    inline ~cWPanel()
    {
      if (Dis)
        OHID->Dis(OHID);
    }


    cWPanel& operator=(const cWPanel&) = delete; // Atanamaz

  };


  class cWButton: public cWVisual
  {
  public:

    // C++
    inline explicit cWButton(wButton nOHID)
    {
      Dis = false;
      OHID = nOHID;
    }

    inline cWButton(const cWButton &Source)
    {
      Dis = false;
      OHID = Source.OHID;
    }

    inline cWButton()
    {
      OHID = Widget.WButton.New();
    }

    inline ~cWButton()
    {
      if (Dis)
        OHID->Dis(OHID);
    }


    cWButton& operator=(const cWButton&) = delete; // Atanamaz

  };


  class cWLabel: public cWVisual
  {
  public:

    // C++
    inline explicit cWLabel(wLabel nOHID)
    {
      Dis = false;
      OHID = nOHID;
    }

    inline cWLabel(const cWLabel &Source)
    {
      Dis = false;
      OHID = Source.OHID;
    }

    inline cWLabel()
    {
      OHID = Widget.WLabel.New();
    }

    inline ~cWLabel()
    {
      if (Dis)
        OHID->Dis(OHID);
    }


    cWLabel& operator=(const cWLabel&) = delete; // Atanamaz

  };


  #pragma endregion



  // Nuc, Native Obj
  #pragma region Window

  using windowSess = struct: sessBase {}*;
  

  struct sWindow
  {
    windowSess (*Start)(screenSess ScrSess);

    wVisual  (*Root)(windowSess WinSess);
  }
  extern Window;



  class cWindowSess
  {
  protected:
    windowSess OHID = Nil;
    bool Dis = true; 

  public:

    // C++
    inline explicit cWindowSess(windowSess nOHID) noexcept : Dis(false), OHID(nOHID)
    {}

    inline cWindowSess(const cWindowSess &Source) noexcept : Dis(false), OHID(Source.OHID)
    {}

    inline explicit cWindowSess(cScreenSess ScreenSess)
    {
      OHID = Window.Start(ScreenSess.GetOHID());
    }

    inline ~cWindowSess()
    {
      if (Dis)
        OHID->Stop(OHID);
    }


    cWindowSess& operator=(const cWindowSess&) = delete; // Atanamaz


  public:
    
    // API
    inline windowSess GetOHID()
    {
      return OHID;
    }


    inline cWVisual  Root()
    {
      return cWVisual(Window.Root(OHID));
    }

  };

  #pragma endregion



  // Nuc, Native Obj
  #pragma region SystemUI

  using systemUISess = struct: sessBase {}*;


  struct sSystemUI
  {
    systemUISess (*Start)(screenSess ScrSess);

  }
  extern SystemUI;



  class cSystemUISess
  {
  protected:
    systemUISess OHID = Nil;
    bool Dis = true; 

  public:

    // C++
    inline explicit cSystemUISess(systemUISess nOHID) noexcept : Dis(false), OHID(nOHID)
    {}

    inline cSystemUISess(const cSystemUISess &Source) noexcept : Dis(false), OHID(Source.OHID)
    {}

    inline explicit cSystemUISess(cScreenSess ScreenSess)
    {
      OHID = SystemUI.Start(ScreenSess.GetOHID());
    }

    inline ~cSystemUISess()
    {
      if (Dis)
        OHID->Stop(OHID);
    }


    cSystemUISess& operator=(const cSystemUISess&) = delete; // Atanamaz


  public:
    
    // API
    inline systemUISess GetOHID()
    {
      return OHID;
    }


  };


  #pragma endregion



  // Nuc
  #pragma region SystemUI/Notify

  struct sSystemUI_Notify
  {
    wVisual (*New)();
  }
  extern SystemUI_Notify;

  #pragma endregion



  // Nuc, Native Obj
  #pragma region SystemUI/Status

  using systemUI_Status_Icon = qobject*;


  struct sSystemUI_Status
  {
    wVisual (*New)();

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



  class cSystemUI_Status_Icon
  {
  protected:
    systemUI_Status_Icon OHID = Nil;
    bool Dis = true; 

  public:

    // C++
    inline explicit cSystemUI_Status_Icon(systemUI_Status_Icon nOHID) noexcept : Dis(false), OHID(nOHID)
    {}

    inline cSystemUI_Status_Icon(const cSystemUI_Status_Icon &Source) noexcept : Dis(false), OHID(Source.OHID)
    {}

    inline cSystemUI_Status_Icon(string Text, cSurface2 Icon)
    {
      OHID = SystemUI_Status.Icon.New(Text.c_str(), Icon.GetOHID());
    }

    inline ~cSystemUI_Status_Icon()
    {
      if (Dis)
        OHID->Dis(OHID);
    }


    cSystemUI_Status_Icon& operator=(const cSystemUI_Status_Icon&) = delete; // Atanamaz


  public:
    
    // API
    inline systemUI_Status_Icon GetOHID()
    {
      return OHID;
    }


    // Prop
    inline cSurface2 Icon()
    {
      return cSurface2(SystemUI_Status.Icon.IconGet(OHID));
    }

    inline void      Icon(cSurface2 Val)
    {
      SystemUI_Status.Icon.IconSet(OHID, Val.GetOHID());
    }


    inline string Text()
    {
      jstring *Cac = SystemUI_Status.Icon.TextGet(OHID);

      string Ret(Cac->Str);
    
      Cac->Dis(Cac);

      return Ret;
    }

    inline void   Text(string Val)
    {
      SystemUI_Status.Icon.TextSet(OHID, Val.c_str());
    }

  };

  #pragma endregion

}
