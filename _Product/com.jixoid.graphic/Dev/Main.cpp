#include <iostream>
#include <cmath>

#include "Basis.hpp"
#include "Nucleol.hpp"

#include "JConf.hpp"

#include <cairo/cairo.h>
#include <string.h>
#include <fontconfig/fontconfig.h>

using namespace std;
using namespace jix;

#define PopNuc(X)  X = *(s##X*)NucCom.Get(#X);
#define PushNuc(X) NucCom = Com; NucCom.Set(#X, &X); X =

#define Log(X)    logger(__func__, X)
#define Log2(X,Y)  logger(__func__, X, Y)



struct __window;

struct __surface2: qobject
{
  // Parent
  __window *Parent;

  // With
  bool With = false;

  // Size
  u16 Width;
  u16 Height;

  // Buffer
  point Buffer;

  // API
  cairo_surface_t *Sur;
  cairo_t *Car;
};

struct __window: qobject
{
  // Size
  u16 Width;
  u16 Height;

  // With
  bool With = false;

  // Sub
  __surface2 *Sur1;
  __surface2 *Sur2;
};




// Surface2
#pragma region Surface2d

#define Self ((__surface2*)__Self)
#define Src  ((__surface2*)__Src)

void      surface2·Dis(surface2 __Self)
{
  if (Self->Parent != Nil)
    return;



  // Surface
  cairo_destroy(Self->Car);
  cairo_surface_destroy(Self->Sur);


  if (Self->With == false)
  {
    if (Self->Buffer != Nil)
      free(Self->Buffer);
  }

  delete Self;
}


surface2  surface2·New(u16 Width, u16 Height)
{
  __surface2* Ret = new __surface2();
  Ret->Dis = &surface2·Dis;

  Ret->Parent = Nil;

  Ret->Buffer = (point)malloc(sizeof(u32) *Width *Height);
  if (!Ret->Buffer)
    Log2("Out of memory", lFatal);


  int Stride = Width *4;
  
  Ret->Sur = cairo_image_surface_create_for_data((unsigned char*)Ret->Buffer, CAIRO_FORMAT_ARGB32, Width, Height, Stride);
  Ret->Car = cairo_create(Ret->Sur);


  Ret->Width  = Width;
  Ret->Height = Height;

  return (surface2)Ret;
}

surface2  surface2·NewWith(u16 Width, u16 Height, point Buffer)
{
  __surface2* Ret = new __surface2();
  Ret->Dis = &surface2·Dis;

  Ret->Parent = Nil;

  // With
  Ret->With = true;
  Ret->Buffer = Buffer;

  int Stride = Width *4;
  
  Ret->Sur = cairo_image_surface_create_for_data((unsigned char*)Ret->Buffer, CAIRO_FORMAT_ARGB32, Width, Height, Stride);
  Ret->Car = cairo_create(Ret->Sur);
  

  Ret->Width  = Width;
  Ret->Height = Height;

  return (surface2)Ret;
}


color     surface2·Pixel(surface2 __Self, point2d Point)
{
  return color(((u32*)Self->Buffer)[(Self->Width *u32(Point.Y)) +u32(Point.X)]);
}

void      surface2·Clear(surface2 __Self)
{
  cairo_set_operator(Self->Car, CAIRO_OPERATOR_SOURCE);
  cairo_set_source_rgba(Self->Car, 0, 0, 0, 0);
  cairo_paint(Self->Car);
  cairo_set_operator(Self->Car, CAIRO_OPERATOR_OVER);
}


size2d    surface2·SizeGet(surface2 __Self)
{
  return {
    .W = f32(Self->Width),
    .H = f32(Self->Height),
  };
}

void      surface2·SizeSet(surface2 __Self, size2d Size)
{
  if (Self->Parent != Nil)
    return;


  // Dispose
  cairo_destroy(Self->Car);
  cairo_surface_destroy(Self->Sur);

  if (Self->Buffer != Nil)
    free(Self->Buffer);


  // Prepare
  size_t
    W = Size.W,
    H = Size.H;


  // Re New
  Self->Buffer = (point)malloc(sizeof(u32) *W *H);
  if (!Self->Buffer)
    Log2("Out of memory", lFatal);

  int Stride = W *4;
  
  Self->Sur = cairo_image_surface_create_for_data((unsigned char*)Self->Buffer, CAIRO_FORMAT_ARGB32, W, H, Stride);
  Self->Car = cairo_create(Self->Sur);


  Self->Width  = W;
  Self->Height = H;
}


point     surface2·BufferGet(surface2 __Self)
{
  return Self->Buffer;
}

void      surface2·BufferSet(surface2 __Self, point Buffer)
{
  if (Self->Parent != Nil)
    return;

  Self->Buffer = Buffer;
}


void      surface2·Draw_Sur2(surface2 __Self, point2d Point, surface2 __Src)
{
  if (Src == Nil)
    Log2("Src Nil pointer", lFatal);


  cairo_set_source_surface(Self->Car, Src->Sur, Point.X, Point.Y);
  cairo_paint(Self->Car);
}

void      surface2·Draw_Bmp(surface2 __Self, point2d Point, const char* Path)
{

  #pragma region Read bmp

  FILE *file = fopen(Path, "rb");
  if (!file)
    Log2("Failed to open file: " +string(strerror(errno)), lFatal);


  u8 Header[54];
  fread(Header, sizeof(u8), 54, file);

  int Width  = *(int*)&Header[18];
  int Height = *(int*)&Header[22];
  int Depth  = *(short*)&Header[28];

  u8  Bit = (Depth /8);

  if (Depth != 24 && Depth != 32) {
    fclose(file);
    Log2("Unsupported format", lFatal);
  }

  u8*  Raw  = (u8*)malloc(Width *Height *Bit);
  u32* Data = (u32*)malloc(sizeof(u32) *Width *Height);

  fseek(file, *(int*)&Header[10], SEEK_SET);
  fread(Raw, sizeof(u8), Width *Height *Bit, file);

  fclose(file);
  
  #pragma endregion

  
  #pragma region Relocation

  // Vert
  for(u32 y = 0; y <= Height -1; y++)
    // Horz
    for (u32 x = 0; x <= Width -1; x++) {
      // Fill
      u32 index = ((Bit *Width *(Height -y -1)) +(Bit *x));
      
      u8 A,R,G,B;

      // 24-bit BMP
      if (Depth == 24) {
        B = Raw[index];
        G = Raw[index +1];
        R = Raw[index +2];
        A = 0xFF;
      }
      // 32-bit BMP
      else if (Depth == 32) {
        B = Raw[index];
        G = Raw[index +1];
        R = Raw[index +2];
        A = Raw[index +3];
      }
      
      Data[(Width *y) +x] = (A << 24) | (R << 16) | (G << 8) | B;
    }

  #pragma endregion



  cairo_surface_t* surface = cairo_image_surface_create_for_data(
    (unsigned char*)Data, CAIRO_FORMAT_ARGB32, Width, Height, Width *4
  );

  cairo_set_source_surface(Self->Car, surface, Point.X, Point.Y);
  cairo_paint(Self->Car);

  cairo_surface_destroy(surface);


  free(Raw);
  free(Data);
}


void      surface2·RectF(surface2 __Self, rect2d Rect, color Color)
{
  cairo_set_source_rgb(Self->Car, Color.R, Color.G, Color.B);
  cairo_rectangle(Self->Car, Rect.L, Rect.T, Rect.W, Rect.H);

  cairo_fill(Self->Car);
}

void      surface2·RectS(surface2 __Self, rect2d Rect, color Color, u16 Width)
{
  // Prepare
  Rect.L += Width /2;
  Rect.T += Width /2;
  Rect.W -= Width;
  Rect.H -= Width;


  // Draw
  cairo_set_source_rgb(Self->Car, Color.R, Color.G, Color.B);
  cairo_set_line_width(Self->Car, Width);

  cairo_rectangle(Self->Car, Rect.L, Rect.T, Rect.W, Rect.H);

  cairo_stroke(Self->Car);
}

void      surface2·RectF_R(surface2 __Self, rect2d Rect, color Color, u16 Round)
{
  if (Round > Rect.W /2.0)
    Round = Rect.W /2.0;
  
  if (Round > Rect.H /2.0)
    Round = Rect.H /2.0;


  cairo_set_source_rgb(Self->Car, Color.R, Color.G, Color.B);
  
  cairo_new_sub_path(Self->Car);
  cairo_arc(Self->Car, Rect.L +Rect.W -Round, Rect.T +Round, Round, -90 *(M_PI /180.0), 0);
  cairo_arc(Self->Car, Rect.L +Rect.W -Round, Rect.T +Rect.H -Round, Round, 0, 90 *(M_PI /180.0));
  cairo_arc(Self->Car, Rect.L +Round, Rect.T +Rect.H -Round, Round, 90 *(M_PI /180.0), 180 *(M_PI /180.0));
  cairo_arc(Self->Car, Rect.L +Round, Rect.T +Round, Round, 180 *(M_PI /180.0), 270 *(M_PI /180.0));
  cairo_close_path(Self->Car);

  cairo_fill(Self->Car);
}

void      surface2·RectS_R(surface2 __Self, rect2d Rect, color Color, u16 Width, u16 Round)
{
  // Prepare
  Rect.L += Width /2;
  Rect.T += Width /2;
  Rect.W -= Width;
  Rect.H -= Width;

  Round -= Width /2;


  if (Round > Rect.W /2.0)
    Round = Rect.W /2.0;
  
  if (Round > Rect.H /2.0)
    Round = Rect.H /2.0;


  // Draw
  cairo_set_source_rgb(Self->Car, Color.R, Color.G, Color.B);
  cairo_set_line_width(Self->Car, Width);
  
  cairo_new_sub_path(Self->Car);
  cairo_arc(Self->Car, Rect.L +Rect.W -Round, Rect.T +Round, Round, -90 *(M_PI /180.0), 0);
  cairo_arc(Self->Car, Rect.L +Rect.W -Round, Rect.T +Rect.H -Round, Round, 0, 90 *(M_PI /180.0));
  cairo_arc(Self->Car, Rect.L +Round, Rect.T +Rect.H -Round, Round, 90 *(M_PI /180.0), 180 *(M_PI /180.0));
  cairo_arc(Self->Car, Rect.L +Round, Rect.T +Round, Round, 180 *(M_PI /180.0), 270 *(M_PI /180.0));
  cairo_close_path(Self->Car);

  cairo_stroke(Self->Car);
}


size2d    surface2·TextSize(surface2 __Self, const char* Text, u16 Width)
{
  cairo_text_extents_t Ex;


  // Draw
  cairo_set_font_size(Self->Car, Width);
  
  cairo_text_extents(Self->Car, Text, &Ex);


  return {
    .W = f32(Ex.width),
    .H = f32(Ex.height),
  };
}

void      surface2·Text(surface2 __Self, const char* Text, u16 Width, point2d Point, color Color)
{
  cairo_text_extents_t Ex;

  // Draw
  cairo_set_source_rgb(Self->Car, Color.R, Color.G, Color.B);
  cairo_set_font_size(Self->Car, Width);
  cairo_text_extents(Self->Car, Text, &Ex);

  cairo_move_to(Self->Car, Point.X, Point.Y +Ex.height);
  cairo_show_text(Self->Car, Text);
}


#undef Self
#undef Src

#pragma endregion



// Window2
#pragma region Window

#define Self  ((__window*)__Self)

void     window·Dis(window __Self)
{
  // Surface
  Self->Sur1->Parent = Nil;
  surface2·Dis(Self->Sur1);

  Self->Sur2->Parent = Nil;
  surface2·Dis(Self->Sur2);


  delete Self;
}


window   window·New(u16 Width, u16 Height)
{
  __window *Ret = new __window();
  Ret->Dis = &window·Dis;

  
  // Meta
  Ret->Width  = Width;
  Ret->Height = Height;


  // Surface
  Ret->Sur1 = (__surface2*)surface2·New(Width, Height);
  Ret->Sur1->Parent = Ret;

  Ret->Sur2 = (__surface2*)surface2·New(Width, Height);
  Ret->Sur2->Parent = Ret;


  return (window)Ret;
}

window   window·NewWith(u16 Width, u16 Height, point Buf1, point Buf2)
{
  __window *Ret = new __window();
  Ret->Dis = &window·Dis;

  
  // Meta
  Ret->Width  = Width;
  Ret->Height = Height;


  // Surface
  Ret->Sur1 = (__surface2*)surface2·NewWith(Width, Height, Buf1);
  Ret->Sur1->Parent = Ret;

  Ret->Sur2 = (__surface2*)surface2·NewWith(Width, Height, Buf2);
  Ret->Sur2->Parent = Ret;


  return (window)Ret;
}


void     window·Swap(window __Self)
{
  // Swap
  __surface2* Cac  = Self->Sur1;
  Self->Sur1       = Self->Sur2;
  Self->Sur2       = Cac;


  // Clear
  cairo_set_operator(Self->Sur1->Car, CAIRO_OPERATOR_SOURCE);
  cairo_set_source_rgba(Self->Sur1->Car, 0, 0, 0, 0);
  cairo_paint(Self->Sur1->Car);
  cairo_set_operator(Self->Sur1->Car, CAIRO_OPERATOR_OVER);
}


surface2 window·Sur(window __Self)
{
  return (surface2)Self->Sur1;
}

surface2 window·Buf(window __Self)
{
  return (surface2)Self->Sur2;
}


size2d   window·SizeGet(window __Self)
{
  return {
    .W = f32(Self->Width),
    .H = f32(Self->Height),
  };
}

void     window·SizeSet(window __Self, size2d Size)
{
  // Surface
  Self->Sur1->Parent = Nil;
  surface2·SizeSet(Self->Sur1, Size);
  Self->Sur1->Parent = Self;

  Self->Sur2->Parent = Nil;
  surface2·SizeSet(Self->Sur2, Size);
  Self->Sur2->Parent = Self;
  

  Self->Width  = Size.W;
  Self->Height = Size.H;
}

#undef Self

#pragma endregion




// Global
sNucCom NucCom;


#pragma region NucCom

void Push(sNucCom Com)
{
  PushNuc(Graphic)
  {
    .Surface2 = {
      .New       = &surface2·New,
      .NewWith   = &surface2·NewWith,

      .Pixel     = &surface2·Pixel,
      .Clear     = &surface2·Clear,

      .SizeGet   = &surface2·SizeGet,
      .SizeSet   = &surface2·SizeSet,

      .BufferGet = &surface2·BufferGet,
      .BufferSet = &surface2·BufferSet,

      .Draw_Sur2 = &surface2·Draw_Sur2,
      .Draw_Bmp  = &surface2·Draw_Bmp,

      .RectF     = &surface2·RectF,
      .RectS     = &surface2·RectS,
      .RectF_R   = &surface2·RectF_R,
      .RectS_R   = &surface2·RectS_R,

      .TextSize  = &surface2·TextSize,
      .Text      = &surface2·Text,
    },

    .Window = {
      .New      = &window·New,
      .NewWith  = &window·NewWith,
      .Swap     = &window·Swap,
      .Sur      = &window·Sur,
      .Buf      = &window·Buf,
      .SizeGet  = &window·SizeGet,
      .SizeSet  = &window·SizeSet,
    },
  };
}


void Load()
{
  // Interface
  vector<string> Fonts;


  #pragma region Font Config

  // System
  {
    JConf::cStc *Conf = JConf::ParseRaw("/System/Conf/Fonts.conf");

    for (auto &Item: Conf->GetStc()["Fonts"]->GetArr())
    {
      string Pkg = Item->GetStc()["Pkg"]->GetVal();

      Fonts.push_back("/System/Moq/" +Pkg+ "/Font");
    }

    delete Conf;
  }


  // Vendor
  {
    JConf::cStc *Conf = JConf::ParseRaw("/Vendor/Conf/Fonts.conf");

    for (auto &Item: Conf->GetStc()["Fonts"]->GetArr())
    {
      string Pkg = Item->GetStc()["Pkg"]->GetVal();

      Fonts.push_back("/Vendor/Moq/" +Pkg+ "/Font");
    }

    delete Conf;
  }

  #pragma endregion



  setenv("FC_CACHE_PATH", "/Cache/Font", 1);


  // Load Fonts
  FcInit();


  // New Config
  FcConfig *Config = FcConfigGetCurrent();
  if (!Config)
  {
    Log2("FontConfig can't initialized", lWarn);
    goto _l_SkipFont;
  }


  // Add Path
  for (auto &X: Fonts)
    if (FcConfigAppFontAddDir(Config, (FcChar8 *)X.c_str()) == FcFalse)
    {
      Log2("FontConfig can't added font", lWarn);
      goto _l_SkipFont;
    }

  _l_SkipFont:
}

void Unload()
{
  FcFini();

}



extern "C" sNucStd NucStd
{
  .Ver = NucVer,

  .Push = &Push,

  .Load   = &Load,
  .Unload = &Unload,
};

#pragma endregion
