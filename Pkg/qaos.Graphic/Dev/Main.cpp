#include <iostream>

#include "Nucleol.hpp"
#include "Graphic.hpp"
#include "Wedling.hpp"

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

  secon Secon;

  int32u Width;
  int32u Height;

  point Data;
};

struct __window2 {
  secon Secon;

  int32u Width;
  int32u Height;

  __surface2* Sur1;
  __surface2* Sur2;
};




// Surface2
#pragma region

surface2  Surface2_New(uid Owner, int32u Width, int32u Height) {

  __surface2* Self = new __surface2;


  // Parent
  Self->Parent = NULL;


  // Wedling
  Self->Secon = Wedling::Secon_New(Owner);

  
  // Meta
  Self->Width  = Width;
  Self->Height = Height;


  // Buffer
  Self->Data = (point)malloc(sizeof(int32u) *Width *Height);


  if (Self->Data == 0)
    throw runtime_error("Can't allocated memory");



  return (point)Self;
}

// Sec: Delete
void      Surface2_Dis(uid Owner, __surface2* Self) {

  if (Self == NULL)
    throw runtime_error("Null pointer");


  // Wedling
  if (!Wedling::Secon_Acc(Self->Secon, Owner, eSecPerm::Delete))
    return;

  Wedling::Secon_Dis(Self->Secon);


  // Surface
  if (Self->Data != NULL)
    free(Self->Data);
  

  delete Self;
}

// Sec: Write
point     Surface2_Ass(uid Owner, __surface2* Self, point New) {

  if (Self == NULL)
    throw runtime_error("Null pointer");


  // Wedling
  if (!Wedling::Secon_Acc(Self->Secon, Owner, eSecPerm::Write))
    return NULL;

  
  point Cac  = Self->Data;
  Self->Data = New;

  return (point)Cac;
}

// Sec: Read
color     Surface2_Get(uid Owner, __surface2* Self, location Point) {
  
  if (Self == NULL)
    throw runtime_error("Null pointer");


  // Wedling
  if (!Wedling::Secon_Acc(Self->Secon, Owner, eSecPerm::Execute))
    return 0x0;


  return ((int32u*)Self->Data)[(Self->Width *Point.Y) +Point.X];
}

// Sec: Execute,  Read
void      Surface2_Draw_Sur2(uid Owner, __surface2* Self, __surface2* Src, location Point) {

  if (Self == NULL)
    throw runtime_error("Null pointer");

  if (Src == NULL)
    throw runtime_error("Null pointer");


  // Wedling
  if (!Wedling::Secon_Acc(Self->Secon, Owner, eSecPerm::Execute))
    return;

  if (!Wedling::Secon_Acc(Src->Secon, Owner, eSecPerm::Read))
    return;



  // Draw
  #pragma region

  int32u BegY = Max(0, Point.Y);
  int32u EndY = Min(Self->Height -1, Point.Y +Src->Height);

  int32u BegX = Max(0, Point.X);
  int32u EndX = Min(Self->Width -1, Point.X +Src->Width);

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

// Sec: Execute
void      Surface2_Draw_bmp(uid Owner, __surface2* Self, string Path, location Point) {

  if (Self == NULL)
    throw runtime_error("Null pointer");


  // Wedling
  if (!Wedling::Secon_Acc(Self->Secon, Owner, eSecPerm::Execute))
    return;



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

// Sec: Execute
void      Surface2_RectF(uid Owner, __surface2* Self, rect Rect, color Color) {

  if (Self == NULL)
    throw runtime_error("Null pointer");

  if (!Wedling::Secon_Acc(Self->Secon, Owner, eSecPerm::Execute))
    return;


  Rect.Right  -= 1;
  Rect.Bottom -= 1;


  // Vert
  for(int32u y = Max(0, Rect.Top); y <= Min(Self->Height -1, Rect.Bottom); y++)
    // Horz
    for (int32u x = Max(0, Rect.Left); x <= Min(Self->Width -1, Rect.Right); x++)
      // Fill
      ((int32u*)Self->Data)[(Self->Width *y) +x] = Color;

}

// Sec: Execute
void      Surface2_RectS(uid Owner, __surface2* Self, rect Rect, color Color, int16u Width) {

  if (Self == NULL)
    throw runtime_error("Null pointer");

  if (!Wedling::Secon_Acc(Self->Secon, Owner, eSecPerm::Execute))
    return;


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

window2  Window2_New(uid Owner, int32u Width, int32u Height) {

  __window2* Self = new __window2;


  // Wedling
  Self->Secon = Wedling::Secon_New(Owner);

  
  // Meta
  Self->Width  = Width;
  Self->Height = Height;


  // Surface
  Self->Sur1 = new __surface2 {
    .Parent = Self,
    .Secon  = Self->Secon,
    .Width  = Width,
    .Height = Height,
    .Data   = (point)malloc(sizeof(int32u) *Width *Height),
  };

  Self->Sur2 = new __surface2 {
    .Parent = Self,
    .Secon  = Self->Secon,
    .Width  = Width,
    .Height = Height,
    .Data   = (point)malloc(sizeof(int32u) *Width *Height),
  };

  if ((Self->Sur1->Data && Self->Sur2->Data) == 0)
    throw runtime_error("Can't allocated memory");



  return (point)Self;
}

// Sec: Delete
void     Window2_Dis(uid Owner, __window2* Self) {

  if (Self == NULL)
    throw runtime_error("Null pointer");


  // Wedling
  if (!Wedling::Secon_Acc(Self->Secon, Owner, eSecPerm::Delete))
    return;

  Wedling::Secon_Dis(Self->Secon);


  // Surface
  if (Self->Sur1->Data != NULL)
    free(Self->Sur1->Data);
  
  if (Self->Sur2->Data != NULL)
    free(Self->Sur2->Data);

  delete Self->Sur1;
  delete Self->Sur2;


  delete Self;
}

// Sec: Execute
void     Window2_Swp(uid Owner, __window2* Self) {

  if (Self == NULL)
    throw runtime_error("Null pointer");


  // Wedling
  if (!Wedling::Secon_Acc(Self->Secon, Owner, eSecPerm::Execute))
    return;


  // Swap
  __surface2* Cac  = Self->Sur1;
  Self->Sur1       = Self->Sur2;
  Self->Sur2       = Cac;
}

// Sec: Read
surface2 Window2_Sur(uid Owner, __window2* Self) {

  if (Self == NULL)
    throw runtime_error("Null pointer");


  // Wedling
  if (!Wedling::Secon_Acc(Self->Secon, Owner, eSecPerm::Read))
    return NULL;

    

  return (point)Self->Sur1;
}

// Sec: Read
surface2 Window2_Buf(uid Owner, __window2* Self) {

  if (Self == NULL)
    throw runtime_error("Null pointer");


  // Wedling
  if (!Wedling::Secon_Acc(Self->Secon, Owner, eSecPerm::Read))
    return NULL;

    
  return (point)Self->Sur2;
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

  ComSet(Window2_New);
  ComSet(Window2_Dis);
  ComSet(Window2_Swp);
  ComSet(Window2_Sur);
  ComSet(Window2_Buf);

  ComSet(Surface2_New);
  ComSet(Surface2_Dis);
  ComSet(Surface2_Ass);
  ComSet(Surface2_Get);

  ComSet(Surface2_Draw_Sur2);
  ComSet(Surface2_Draw_bmp);

  ComSet(Surface2_RectF);
  ComSet(Surface2_RectS);
}

extern "C" void NucPop() {

  Wedling::PopNuc(NucCom);
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
