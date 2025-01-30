#include <iostream>

#include "Nucleol.hpp"
#include "Graphic.hpp"

extern "C" {
  #include <string.h>
}

using namespace std;
using namespace jix;


#define ComSet(X) NucCom.Set(#X, (point)X);



int32u Max(int32u P1, int32u P2) {

  if(P1 >= P2)
    return P1;

  else
    return P2;
}

int32u Min(int32u P1, int32u P2) {

  if(P1 <= P2)
    return P1;
    
  else
    return P2;
}



struct __window2;

struct __surface2 {
  __window2* Parent;

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

surface2  Surface2_New(int32u Width, int32u Height) {

  __surface2* Self = new __surface2;


  // Parent
  Self->Parent = NULL;

  
  // Meta
  Self->Width  = Width;
  Self->Height = Height;


  // Buffer
  Self->Data = (point)malloc(sizeof(int32u) *Width *Height);


  if (Self->Data == 0)
    throw runtime_error("Can't allocated memory");



  return (point)Self;
}

surface2  Surface2_NewWith(int32u Width, int32u Height, point Data) {

  __surface2* Self = new __surface2;


  // Parent
  Self->Parent = NULL;

  
  // Meta
  Self->Width  = Width;
  Self->Height = Height;


  // Buffer
  Self->Data = Data;


  return (point)Self;
}

void      Surface2_Dis(__surface2* Self) {

  if (Self == NULL)
    throw runtime_error("Null pointer");


  // Surface
  if (Self->Data != NULL)
    free(Self->Data);
  

  delete Self;
}

void      Surface2_DisWith(__surface2* Self) {

  if (Self == NULL)
    throw runtime_error("Null pointer");


  delete Self;
}

color     Surface2_Get(__surface2* Self, point2d Point) {
  
  if (Self == NULL)
    throw runtime_error("Null pointer");


  return color(((int32u*)Self->Data)[(Self->Width *Point.Y) +Point.X]);
}

size2d    Surface2_SizeGet(__surface2* Self) {

  if (Self == NULL)
    throw runtime_error("Null pointer");


  return {
    .W = Self->Width,
    .H = Self->Height,
  };
}

void      Surface2_SizeSet(__surface2* Self, size2d Size) {

  if (Self == NULL)
    throw runtime_error("Null pointer");


  if (Self->Parent != NULL)
    return;


  Self->Data = (point)realloc(Self->Data, sizeof(int32u) *Size.Width *Size.Height);

  if (Self->Data == 0)
    throw runtime_error("Can't allocated memory");


  Self->Width  = Size.Width;
  Self->Height = Size.Height;
}


void      Surface2_Draw_Sur2(__surface2* Self, __surface2* Src, point2d Point) {

  if (Self == NULL)
    throw runtime_error("Null pointer");

  if (Src == NULL)
    throw runtime_error("Null pointer");



  // Draw
  #pragma region

  int32u BegY = Max(0, Point.Y);
  int32u EndY = Min(Self->Height -1, Point.Y +Src->Height -1);

  int32u BegX = Max(0, Point.X);
  int32u EndX = Min(Self->Width -1, Point.X +Src->Width -1);

  int32u Pitch = (Src->Width *4);

  // Vert
  for(int32u y = BegY; y <= EndY; y++)
    // Horz
    for (int32u x = BegX; x <= EndX; x++)
      // Fill
      ((int32u*)Self->Data)[(Self->Width *y) +x] =
        ((int32u*)Src->Data)[(Src->Width *(y -BegY)) +x -BegX];


  #pragma endregion

}

void      Surface2_Draw_bmp(__surface2* Self, string Path, point2d Point) {

  if (Self == NULL)
    throw runtime_error("Null pointer");



  // Read bmp
  #pragma region 

  FILE *file = fopen(Path.c_str(), "rb");
  if (!file)
    throw runtime_error("Failed to open file: " +string(strerror(errno)));


  int8u Header[54];
  fread(Header, sizeof(int8u), 54, file);

  int Width  = *(int*)&Header[18];
  int Height = *(int*)&Header[22];
  int Depth  = *(short*)&Header[28];

  if (Depth != 24 && Depth != 32) {
    fclose(file);
    throw runtime_error("Unsupported format");
  }

  int8u  Bit = (Depth /8);
  int32u Size = Width *Height *Bit;

  int8u* Data = (int8u*)malloc(Width *Height *Bit);

  fseek(file, *(int*)&Header[10], SEEK_SET);
  fread(Data, sizeof(int8u), Width *Height *Bit, file);

  fclose(file);
  
  #pragma endregion



  // Draw
  #pragma region

  int32u BegY = Max(0, Point.Y);
  int32u EndY = Min(Self->Height -1, Point.Y +Height);

  int32u BegX = Max(0, Point.X); 
  int32u EndX = Min(Self->Width -1, Point.X +Width); 

  // Vert
  for(int32u y = BegY; y <= EndY; y++)
    // Horz
    for (int32u x = BegX; x <= EndX; x++) {
      // Fill
      int32u index = ((Bit *Width *(Height -y -1 +BegY)) +(Bit *(x -BegX)));

      int8u A,R,G,B;

      // 24-bit BMP
      if (Depth == 24) {
        B = Data[index];
        G = Data[index +1];
        R = Data[index +2];
      }
      // 32-bit BMP
      else if (Depth == 32) {
        B = Data[index];
        G = Data[index +1];
        R = Data[index +2];
        A = Data[index +3];
      }

      int32u Color = (R << 16) | (G << 8) | B;

      ((int32u*)Self->Data)[(Self->Width *y) +x] = Color;
    }


  free(Data);

  #pragma endregion

}


void      Surface2_RectF(__surface2* Self, rect2d Rect, color Color) {

  if (Self == NULL)
    throw runtime_error("Null pointer");


  Rect.Right  -= 1;
  Rect.Bottom -= 1;


  // Vert
  for(int32u y = Max(0, Rect.Top); y <= Min(Self->Height -1, Rect.Bottom); y++)
    // Horz
    for (int32u x = Max(0, Rect.Left); x <= Min(Self->Width -1, Rect.Right); x++)
      // Fill
      ((int32u*)Self->Data)[(Self->Width *y) +x] = Color;

}

void      Surface2_RectB(__surface2* Self, rect2d Rect, color Color, int16u Width) {

  if (Self == NULL)
    throw runtime_error("Null pointer");


  Rect.Right  -= 1;
  Rect.Bottom -= 1;



  // Horz lines
  for (int32u x = Max(0, Rect.Left); x <= Min(Self->Width -1, Rect.Right); x++) {
    
    if (Rect.T >= 0 && Rect.T <= Self->Height -1)
      ((int32u*)Self->Data)[(Self->Width *Rect.T) +x] = Color;

    if (Rect.B >= 0 && Rect.B <= Self->Height -1)
      ((int32u*)Self->Data)[(Self->Width *Rect.B) +x] = Color;

  }


  // Vert lines
  for(int32u y = Max(0, Rect.Top); y <= Min(Self->Height -1, Rect.Bottom); y++) {

    if (Rect.L >= 0 && Rect.L <= Self->Width -1)
      ((int32u*)Self->Data)[(Self->Width *y) +Rect.L] = Color;

    if (Rect.R >= 0 && Rect.R <= Self->Width -1)
      ((int32u*)Self->Data)[(Self->Width *y) +Rect.R] = Color;

  }

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
  Self->Sur1 = new __surface2 {
    .Parent = Self,
    .Width  = Width,
    .Height = Height,
    .Data   = (point)malloc(sizeof(int32u) *Width *Height),
  };

  Self->Sur2 = new __surface2 {
    .Parent = Self,
    .Width  = Width,
    .Height = Height,
    .Data   = (point)malloc(sizeof(int32u) *Width *Height),
  };

  if ((Self->Sur1->Data && Self->Sur2->Data) == 0)
    throw runtime_error("Can't allocated memory");



  return (point)Self;
}

window2  Window2_NewWith(int32u Width, int32u Height, point Data1, point Data2) {

  __window2* Self = new __window2;

  
  // Meta
  Self->Width  = Width;
  Self->Height = Height;


  // Surface
  Self->Sur1 = new __surface2 {
    .Parent = Self,
    .Width  = Width,
    .Height = Height,
    .Data   = Data1,
  };

  Self->Sur2 = new __surface2 {
    .Parent = Self,
    .Width  = Width,
    .Height = Height,
    .Data   = Data2,
  };

  return (point)Self;
}

void     Window2_Dis(__window2* Self) {

  if (Self == NULL)
    throw runtime_error("Null pointer");


  // Surface
  if (Self->Sur1->Data != NULL)
    free(Self->Sur1->Data);
  
  if (Self->Sur2->Data != NULL)
    free(Self->Sur2->Data);

  delete Self->Sur1;
  delete Self->Sur2;


  delete Self;
}

void     Window2_DisWith(__window2* Self) {

  if (Self == NULL)
    throw runtime_error("Null pointer");


  // Surface
  delete Self->Sur1;
  delete Self->Sur2;


  delete Self;
}

void     Window2_Swp(__window2* Self) {

  if (Self == NULL)
    throw runtime_error("Null pointer");


  // Swap
  __surface2* Cac  = Self->Sur1;
  Self->Sur1       = Self->Sur2;
  Self->Sur2       = Cac;
}

surface2 Window2_Sur(__window2* Self) {

  if (Self == NULL)
    throw runtime_error("Null pointer");

    

  return (point)Self->Sur1;
}

surface2 Window2_Buf(__window2* Self) {

  if (Self == NULL)
    throw runtime_error("Null pointer");

    
  return (point)Self->Sur2;
}

size2d   Window2_SizeGet(__window2* Self) {

  if (Self == NULL)
    throw runtime_error("Null pointer");


  return {
    .W = Self->Width,
    .H = Self->Height,
  };
}

void     Window2_SizeSet(__window2* Self, size2d Size) {

  if (Self == NULL)
    throw runtime_error("Null pointer");



  // Surface   
  Self->Sur1->Data = (point)realloc(Self->Sur1->Data, sizeof(int32u) *Size.Width *Size.Height);
  Self->Sur2->Data = (point)realloc(Self->Sur2->Data, sizeof(int32u) *Size.Width *Size.Height);

  if ((Self->Sur1->Data && Self->Sur2->Data) == 0)
    throw runtime_error("Can't allocated memory");



  Self->Sur1->Width  = Size.Width;
  Self->Sur1->Height = Size.Height;

  Self->Sur2->Width  = Size.Width;
  Self->Sur2->Height = Size.Height;


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
