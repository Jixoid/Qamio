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
#include "JConf.h"
#include "Kernel.hh"

#include "HAL/Graphic.hh"

#include <cairo/cairo.h>
#include <string.h>
#include <fontconfig/fontconfig.h>

using namespace std;
using namespace jix;
using namespace hal;

using namespace qaos;

#define Log(X)     kernel::Logger(__func__, X)
#define Log2(X,Y)  kernel::Logger(__func__, X, Y)



struct __surface2
{
  // API
  cairo_t *Car;
  cairo_surface_t *Sur;

  // Size
  u16 Width;
  u16 Height;

  // Buffer
  bool  DisBuf = false;
  point Buffer = Nil;
};



// Global
graphic::sDriver DRV = {

  #define Self ((__surface2*)__Self)
  #define Src  ((__surface2*)__Src)


  // New Dis
  .New = [](u32, u16 Width, u16 Height) -> point
  {
    __surface2* Ret = new __surface2();
    
    Ret->Sur = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, Width, Height);
    Ret->Car = cairo_create(Ret->Sur);


    Ret->Width  = Width;
    Ret->Height = Height;

    return (point)Ret;
  },

  .Dis = [](point __Self)
  {
    // Surface
    cairo_destroy(Self->Car);
    cairo_surface_destroy(Self->Sur);

    // Free
    if (Self->DisBuf)
      free(Self->Buffer);
    
    delete Self;
  },


  // New With
  .New_With_Mem = [](u32, u16 Width, u16 Height, point Buffer) -> point
  {
    __surface2* Ret = new __surface2();

    // With
    Ret->Buffer = Buffer;

    Ret->Sur = cairo_image_surface_create_for_data((unsigned char*)Ret->Buffer, CAIRO_FORMAT_ARGB32, Width, Height, Width *4);
    Ret->Car = cairo_create(Ret->Sur);


    Ret->Width  = Width;
    Ret->Height = Height;

    return (point)Ret;
  },

  .New_With_Sur = [](u32, point __Src) -> point
  {
    __surface2* Ret = new __surface2();


    Ret->Sur = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, Src->Width, Src->Height);
    Ret->Car = cairo_create(Ret->Sur);


    Ret->Width  = Src->Width;
    Ret->Height = Src->Height;


    // Copy
    cairo_set_source_surface(Ret->Car, Src->Sur, 0,0);
    cairo_paint(Ret->Car);

    return (point)Ret;
  },

  .New_With_Png = [](u32, const char* Path) -> point
  {
    __surface2* Ret = new __surface2();


    Ret->Sur = cairo_image_surface_create_from_png(Path);
    Ret->Car = cairo_create(Ret->Sur);


    Ret->Width  = cairo_image_surface_get_width(Ret->Sur);
    Ret->Height = cairo_image_surface_get_height(Ret->Sur);

    return (point)Ret;
  },

  .New_With_Bmp = [](u32, const char* Path) -> point
  {
    u32 *Data;
    u8  *Raw;

    int Width;
    int Height;


    #pragma region Read bmp

    FILE *file = fopen(Path, "rb");
    if (!file)
      return Nil;


    u8 Header[54];
    fread(Header, sizeof(u8), 54, file);

    Width  = *(int*)&Header[18];
    Height = *(int*)&Header[22];
    int Depth  = *(short*)&Header[28];

    u8  Bit = (Depth /8);

    if (Depth != 24 && Depth != 32)
    {
      fclose(file);
      return Nil;
    }

    Raw  = (u8*)malloc(Width *Height *Bit);
    Data = (u32*)malloc(sizeof(u32) *Width *Height);

    fseek(file, *(int*)&Header[10], SEEK_SET);
    fread(Raw, sizeof(u8), Width *Height *Bit, file);

    fclose(file);
    
    #pragma endregion

    
    #pragma region Relocation

    // Vert
    for(u32 y = 0; y <= Height -1; y++)
      // Horz
      for (u32 x = 0; x <= Width -1; x++)
      {
        // Fill
        u32 index = ((Bit *Width *(Height -y -1)) +(Bit *x));
        
        u8 A,R,G,B;

        // 24-bit BMP
        if (Depth == 24)
        {
          B = Raw[index];
          G = Raw[index +1];
          R = Raw[index +2];
          A = 0xFF;
        }
        // 32-bit BMP
        ef (Depth == 32)
        {
          B = Raw[index];
          G = Raw[index +1];
          R = Raw[index +2];
          A = Raw[index +3];
        }
        
        Data[(Width *y) +x] = (u32)(A << 24) | (u32)(R << 16) | (u32)(G << 8) | (u32)B;
      }

    #pragma endregion

    free(Raw);


    // New
    __surface2* Ret = (__surface2*)DRV.New_With_Mem(0, Width, Height, Data);

    Ret->DisBuf = true;
    
    return Ret;
  },


  // API
  .Pixel = [](point __Self, graphic::point2d Point) -> graphic::color
  {
    u32* Data = (u32*)cairo_image_surface_get_data(Self->Sur);

    u32 Ret = *(u32*)(Data +((u16)Point.Y *4) +(u16)Point.X);
    
    return graphic::color(Ret);
  },

  .Clear = [](point __Self)
  {
    cairo_set_operator(Self->Car, CAIRO_OPERATOR_SOURCE);
    
    cairo_set_source_rgba(Self->Car, 0,0,0,0);
    cairo_paint(Self->Car);

    cairo_set_operator(Self->Car, CAIRO_OPERATOR_OVER);
  },


  // Size
  .SizeGet = [](point __Self) -> graphic::size2d
  {
    return {
      .W = f32(Self->Width),
      .H = f32(Self->Height),
    };
  },

  .SizeSet = [](point __Self, u16 Width, u16 Height)
  {
    // Dispose
    cairo_destroy(Self->Car);
    cairo_surface_destroy(Self->Sur);

    if (Self->DisBuf)
      free(Self->Buffer);


    Self->DisBuf = false;
    Self->Buffer = Nil;


    // Re-New
    Self->Sur = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, Width, Height);
    Self->Car = cairo_create(Self->Sur);


    Self->Width  = Width;
    Self->Height = Height;
  },


  // Pre Config
  .Set_Source_RGB = [](point __Self, graphic::color Color)
  {
    cairo_set_source_rgb(Self->Car, Color.R, Color.G, Color.B);
  },

  .Set_Source_RGBA = [](point __Self, graphic::color Color)
  {
    cairo_set_source_rgba(Self->Car, Color.R, Color.G, Color.B, Color.A);
  },

  .Set_Source_Sur = [](point __Self, point __Src, graphic::point2d Point)
  {
    cairo_set_source_surface(Self->Car, Src->Sur, Point.X, Point.Y);
  },


  .Set_Font_Size = [](point __Self, f32 Size)
  {
    cairo_set_font_size(Self->Car, Size);
  },


  // Basis
  .Basis_Move = [](point __Self, graphic::point2d Point)
  {
    cairo_move_to(Self->Car, Point.X, Point.Y);
  },


  .Basis_Line = [](point __Self, graphic::point2d Point)
  {
    cairo_line_to(Self->Car, Point.X, Point.Y);
  },

  .Basis_Rect = [](point __Self, graphic::rect2d Rect)
  {
    cairo_rectangle(Self->Car, Rect.X, Rect.Y, Rect.W, Rect.H);
  },

  .Basis_Arc = [](point __Self, graphic::point2d Point, f32 Round, f32 Angle1, f32 Angle2)
  {
    cairo_arc(Self->Car, Point.X, Point.Y, Round, Angle1, Angle2);
  },


  .Basis_RectR = [](point __Self, graphic::rect2d Rect, f32 Round)
  {
    if (Round > Rect.W /2.0)
      Round = Rect.W /2.0;
    
    if (Round > Rect.H /2.0)
      Round = Rect.H /2.0;


    cairo_new_sub_path(Self->Car);
    cairo_arc(Self->Car, Rect.X +Rect.W -Round, Rect.Y +Round, Round, -90 *(M_PI /180.0), 0);
    cairo_arc(Self->Car, Rect.X +Rect.W -Round, Rect.Y +Rect.H -Round, Round, 0, 90 *(M_PI /180.0));
    cairo_arc(Self->Car, Rect.X +Round, Rect.Y +Rect.H -Round, Round, 90 *(M_PI /180.0), 180 *(M_PI /180.0));
    cairo_arc(Self->Car, Rect.X +Round, Rect.Y +Round, Round, 180 *(M_PI /180.0), 270 *(M_PI /180.0));
    cairo_close_path(Self->Car);
  },


  // Text
  .Text_Size = [](point __Self, const char* Text) -> graphic::size2d
  {
    cairo_text_extents_t Ex;

    // Get
    cairo_text_extents(Self->Car, Text, &Ex);


    return {
      .W = f32(Ex.width),
      .H = f32(Ex.height),
    };
  },
  
  .Text = [](point __Self, const char* Text)
  {
    cairo_text_extents_t Ex;

    // Draw
    cairo_text_extents(Self->Car, Text, &Ex);

    cairo_rel_move_to(Self->Car, 0, Ex.height);
    cairo_show_text(Self->Car, Text);
  },


  // OK
  .Flush = [](point __Self)
  {
    cairo_surface_flush(Self->Sur);
  },

  .Paint = [](point __Self)
  {
    cairo_paint(Self->Car);
  },

  .Paint_A = [](point __Self, f32 Alpha)
  {
    cairo_paint_with_alpha(Self->Car, Alpha);
  },

  .Fill = [](point __Self)
  {
    cairo_fill(Self->Car);
  },

  .Stroke = [](point __Self)
  {
    cairo_stroke(Self->Car);
  },


  #undef Self
  #undef Src
};




// Nuc Control
graphic::sHAL *HAL = Nil;


extern "C" kernel::nucStd NucStd
{
  .Ver = kernel::NucVer,


  .Load = []()
  {
    // Interface
    vector<string> Fonts;


    for (string Part: {"System", "Vendor"})
    {
      jc_stc Conf = jc_Parse(("/"+Part+"/Conf/Fonts.conf").c_str());
      if (!jc_IsStc(Conf))
        Log2("Corrupt config file: "+("/"+Part+"/Conf/Fonts.conf"), kernel::lPanic);


      jc_arr CFonts = jc_StcGet(Conf, "Fonts");
      if (!jc_IsArr(CFonts))
        Log2("Corrupt config file: "+("/"+Part+"/Conf/Fonts.conf"), kernel::lPanic);


      for (int i = 0; i < jc_ArrC(CFonts); i++)
      {
        jc_obj Cac;
        char *CStr;

        jc_stc Item = jc_ArrGet(CFonts, i);
        if (!jc_IsStc(Item))
          Log2("Corrupt config file: "+("/"+Part+"/Conf/Fonts.conf"), kernel::lPanic);


        // .Pkg
        Cac = jc_StcGet(Item, "Pkg");
        if (!jc_IsVal(Cac))
          Log2("Corrupt config file: "+("/"+Part+"/Conf/Fonts.conf"), kernel::lPanic);
        
        CStr = jc_ValGet(Cac);
        string Pkg = string(CStr);
        jc_DisStr(CStr);


        Fonts.push_back("/"+Part+"/Moq/"+Pkg+"/Font");
      }

      jc_DisStc(Conf);
    }


    // Prepare
    setenv("FC_CACHE_PATH", "/Cache/Font", 1);


    // Load Fonts
    FcInit();

    string CFile;


    // New Config
    FcConfig *Config = FcConfigGetCurrent();
    if (!Config)
    {
      Log2("FontConfig can't initialized", kernel::lWarn);
      goto _l_SkipFont;
    }


    // Add Path
    for (auto &X: Fonts)
      if (FcConfigAppFontAddDir(Config, (FcChar8*)X.c_str()) == FcFalse)
      {
        Log2("FontConfig can't added font", kernel::lWarn);
        goto _l_SkipFont;
      }


    // Load config
    CFile = R"(<?xml version="1.0"?>
<!DOCTYPE fontconfig SYSTEM "urn:fontconfig:fonts.dtd">
<fontconfig>
    
</fontconfig>
)";

    if (FcConfigParseAndLoadFromMemory(Config, (FcChar8*)CFile.c_str(), true) == FcFalse)
    {
      Log2("FontConfig can't load fontconfig", kernel::lWarn);
      goto _l_SkipFont;
    }
    

    _l_SkipFont: {}
  },

  .Unload = []()
  {
    FcInit();

    if (HAL != Nil)
      HAL->DelDriver(&DRV);

  },


  .Pop = []()
  {
    HAL = (decltype(HAL))kernel::NucGet(graphic::Domain);
    if (HAL == Nil)
    {
      Log2(string(graphic::Domain) +" is not found", kernel::lWarn);
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
