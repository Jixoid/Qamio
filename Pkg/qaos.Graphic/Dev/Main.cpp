#include <iostream>

#include "Nucleol.hpp"
#include "Graphic.hpp"

extern "C" {
  #include <cairo/cairo.h>
  #include <GL/gl.h>
  #include <string.h>
  #include <stb_image.h>
}

using namespace std;
using namespace jix;


#define ComSet(X) NucCom.Set(#X, (point)X);


struct __window2;

struct __surface2 {
  __window2* Parent;

  cairo_surface_t* Sur;
  cairo_t* Car;

  int32u Width;
  int32u Height;

  point Data;
};

struct __window2 {

  int32u Width;
  int32u Height;

  __surface2* Sur1;
  __surface2* Sur2;
};




// Surface2
#pragma region

__surface2* Surface2_New(int32u Width, int32u Height) {

  __surface2* Self = new __surface2;

  Self->Parent = Nil;

  Self->Data = (point)malloc(sizeof(int32u) *Width *Height);
  if (!Self->Data)
    throw runtime_error("Can't allocated memory");

  int stride = Width *4;
  
  Self->Sur = cairo_image_surface_create_for_data((unsigned char*)Self->Data, CAIRO_FORMAT_ARGB32, Width, Height, stride);
  Self->Car = cairo_create(Self->Sur);

  cairo_set_source_rgb(Self->Car, 0, 0, 0);
  cairo_paint(Self->Car);


  Self->Width  = Width;
  Self->Height = Height;

  return Self;
}

__surface2* Surface2_NewWith(int32u Width, int32u Height, point Data) {

  __surface2* Self = new __surface2;

  Self->Parent = Nil;

  Self->Data = Data;

  int stride = Width *4;
  
  Self->Sur = cairo_image_surface_create_for_data((unsigned char*)Self->Data, CAIRO_FORMAT_ARGB32, Width, Height, stride);
  Self->Car = cairo_create(Self->Sur);

  cairo_set_source_rgb(Self->Car, 0, 0, 0);
  cairo_paint(Self->Car);


  Self->Width  = Width;
  Self->Height = Height;

  return Self;
}

void      Surface2_Dis(__surface2* Self) {

  if (Self == Nil)
    throw runtime_error("Nil pointer");


  if (Self->Parent != Nil)
    return;



  // Surface
  cairo_destroy(Self->Car);
  cairo_surface_destroy(Self->Sur);


  if (Self->Data != Nil)
    free(Self->Data);

  delete Self;
}

void      Surface2_DisWith(__surface2* Self) {

  if (Self == Nil)
    throw runtime_error("Nil pointer");


  if (Self->Parent != Nil)
    return;



  // Surface
  cairo_destroy(Self->Car);
  cairo_surface_destroy(Self->Sur);


  delete Self;
}

color     Surface2_Get(__surface2* Self, point2d Point) {
  
  if (Self == Nil)
    throw runtime_error("Nil pointer");


  return color(((int32u*)Self->Data)[(Self->Width *int32u(Point.Y)) +int32u(Point.X)]);
}

size2d    Surface2_SizeGet(__surface2* Self) {

  if (Self == Nil)
    throw runtime_error("Nil pointer");


  return {
    .W = Self->Width,
    .H = Self->Height,
  };
}

void      Surface2_SizeSet(__surface2* Self, size2d Size) {

  if (Self == Nil)
    throw runtime_error("Nil pointer");


  if (Self->Parent != Nil)
    return;


  // Dispose
  cairo_destroy(Self->Car);
  cairo_surface_destroy(Self->Sur);

  if (Self->Data != Nil)
    free(Self->Data);

  // Re New
  Self->Data = (point)malloc(sizeof(int32u) *Size.Width *Size.Height);
  if (!Self->Data)
    throw runtime_error("Can't allocated memory");

  int stride = Size.Width *4;
  
  Self->Sur = cairo_image_surface_create_for_data((unsigned char*)Self->Data, CAIRO_FORMAT_ARGB32, Size.Width, Size.Height, stride);
  Self->Car = cairo_create(Self->Sur);

  cairo_set_source_rgb(Self->Car, 0, 0, 0);
  cairo_paint(Self->Car);


  Self->Width  = Size.Width;
  Self->Height = Size.Height;
}


void      Surface2_Draw_Sur2(__surface2* Self, __surface2* Src, point2d Point) {

  if (Self == Nil)
    throw runtime_error("Nil pointer");

  if (Src == Nil)
    throw runtime_error("Nil pointer");


  cairo_set_source_surface(Self->Car, Src->Sur, Point.X, Point.Y);
  cairo_paint(Self->Car);
}

void      Surface2_Draw_bmp(__surface2* Self, string Path, point2d Point) {

  if (Self == Nil)
    throw runtime_error("Nil pointer");


  #pragma region Read bmp

  FILE *file = fopen(Path.c_str(), "rb");
  if (!file)
    throw runtime_error("Failed to open file: " +string(strerror(errno)));


  int8u Header[54];
  fread(Header, sizeof(int8u), 54, file);

  int Width  = *(int*)&Header[18];
  int Height = *(int*)&Header[22];
  int Depth  = *(short*)&Header[28];

  int8u  Bit = (Depth /8);

  if (Depth != 24 && Depth != 32) {
    fclose(file);
    throw runtime_error("Unsupported format");
  }

  int8u*  Raw  = (int8u*)malloc(Width *Height *Bit);
  int32u* Data = (int32u*)malloc(sizeof(int32u) *Width *Height);

  fseek(file, *(int*)&Header[10], SEEK_SET);
  fread(Raw, sizeof(int8u), Width *Height *Bit, file);

  fclose(file);
  
  #pragma endregion

  
  #pragma region Relocation

  // Vert
  for(int32u y = 0; y <= Height -1; y++)
    // Horz
    for (int32u x = 0; x <= Width -1; x++) {
      // Fill
      int32u index = ((Bit *Width *(Height -y -1)) +(Bit *x));
      
      int8u A,R,G,B;

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


void      Surface2_RectF(__surface2* Self, rect2d Rect, color Color) {

  if (Self == Nil)
    throw runtime_error("Nil pointer");


  cairo_set_source_rgb(Self->Car, Color.R, Color.G, Color.B);
  
  cairo_rectangle(Self->Car, Rect.L, Rect.T, Rect.W, Rect.H);
  cairo_fill(Self->Car);
}

void      Surface2_RectB(__surface2* Self, rect2d Rect, color Color, int16u Width) {

  if (Self == Nil)
    throw runtime_error("Nil pointer");

  cairo_set_source_rgb(Self->Car, Color.R, Color.G, Color.B);
  cairo_set_line_width(Self->Car, Width);

  cairo_rectangle(Self->Car, Rect.L, Rect.T, Rect.W, Rect.H);
  cairo_stroke(Self->Car);
}

#pragma endregion




// Window2
#pragma region

window2  Window2_New(int32u Width, int32u Height) {

  __window2* Self = new __window2;

  
  // Meta
  Self->Width  = Width;
  Self->Height = Height;


  // Surface
  Self->Sur1 = Surface2_New(Width, Height);
  Self->Sur1->Parent = Self;

  Self->Sur2 = Surface2_New(Width, Height);
  Self->Sur2->Parent = Self;


  return (point)Self;
}

window2  Window2_NewWith(int32u Width, int32u Height, point Data1, point Data2) {

  __window2* Self = new __window2;

  
  // Meta
  Self->Width  = Width;
  Self->Height = Height;


  // Surface
  Self->Sur1 = Surface2_NewWith(Width, Height, Data1);
  Self->Sur1->Parent = Self;

  Self->Sur2 = Surface2_NewWith(Width, Height, Data2);
  Self->Sur2->Parent = Self;


  return (point)Self;
}

void     Window2_Dis(__window2* Self) {

  if (Self == Nil)
    throw runtime_error("Nil pointer");


  // Surface
  Self->Sur1->Parent = Nil;
  Surface2_Dis(Self->Sur1);

  Self->Sur2->Parent = Nil;
  Surface2_Dis(Self->Sur2);


  delete Self;
}

void     Window2_DisWith(__window2* Self) {

  if (Self == Nil)
    throw runtime_error("Nil pointer");


  // Surface
  Self->Sur1->Parent = Nil;
  Surface2_DisWith(Self->Sur1);

  Self->Sur2->Parent = Nil;
  Surface2_DisWith(Self->Sur2);


  delete Self;
}

void     Window2_Swp(__window2* Self) {

  if (Self == Nil)
    throw runtime_error("Nil pointer");


  cairo_surface_flush(Self->Sur1->Sur);


  // Swap
  __surface2* Cac  = Self->Sur1;
  Self->Sur1       = Self->Sur2;
  Self->Sur2       = Cac;
}

surface2 Window2_Sur(__window2* Self) {

  if (Self == Nil)
    throw runtime_error("Nil pointer");

    

  return (point)Self->Sur1;
}

surface2 Window2_Buf(__window2* Self) {

  if (Self == Nil)
    throw runtime_error("Nil pointer");

    
  return (point)Self->Sur2;
}

size2d   Window2_SizeGet(__window2* Self) {

  if (Self == Nil)
    throw runtime_error("Nil pointer");


  return {
    .W = Self->Width,
    .H = Self->Height,
  };
}

void     Window2_SizeSet(__window2* Self, size2d Size) {

  if (Self == Nil)
    throw runtime_error("Nil pointer");



  // Surface
  Self->Sur1->Parent = Nil;
  Surface2_SizeSet(Self->Sur1, Size);
  Self->Sur1->Parent = Self;

  Self->Sur2->Parent = Nil;
  Surface2_SizeSet(Self->Sur2, Size);
  Self->Sur2->Parent = Self;
  

  Self->Width  = Size.Width;
  Self->Height = Size.Height;
}

#pragma endregion







// Global
sNucCom NucCom;


#pragma region NucCom
extern "C" bool NucCheck(int16u Ver) {
  return (gnucVer == Ver);
}


extern "C" void NucPush(sNucCom Com) {

  NucCom = Com;

  // Surface
  ComSet(Surface2_New);
  ComSet(Surface2_NewWith);
  ComSet(Surface2_Dis);
  ComSet(Surface2_DisWith);
  ComSet(Surface2_Get);
  ComSet(Surface2_SizeGet);
  ComSet(Surface2_SizeSet);

  ComSet(Surface2_Draw_Sur2);
  ComSet(Surface2_Draw_bmp);

  ComSet(Surface2_RectF);
  ComSet(Surface2_RectB);


  // Window
  ComSet(Window2_New);
  ComSet(Window2_NewWith);
  ComSet(Window2_Dis);
  ComSet(Window2_DisWith);
  ComSet(Window2_Swp);
  ComSet(Window2_Sur);
  ComSet(Window2_Buf);
  ComSet(Window2_SizeGet);
  ComSet(Window2_SizeSet);
}

extern "C" void NucPop() {

}


extern "C" void NucLoad() {
  
}

extern "C" void NucUnload() {

}


extern "C" void NucStart() {
  
}

extern "C" void NucStop() {
  
}

#pragma endregion
