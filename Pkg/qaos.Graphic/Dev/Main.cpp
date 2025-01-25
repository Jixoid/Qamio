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



struct __window2 {
  secon Secon;

  int32u Width;
  int32u Height;

  point  Buf1, Buf2;
};


window2 Window2_New(uid nOwner, int32u nWidth, int32u nHeight) {

  __window2* Self = new __window2;


  Self->Secon = Wedling::Secon_New(nOwner);

  Self->Width  = nWidth;
  Self->Height = nHeight;

  Self->Buf1 = (point)malloc(sizeof(int32u) *nWidth *nHeight);
  Self->Buf2 = (point)malloc(sizeof(int32u) *nWidth *nHeight);

  if ((Self->Buf1 && Self->Buf2) == 0)
    throw runtime_error("Can't allocated memory");


  return (point)Self;
}

void    Window2_Dis(__window2* Self) {

  if (Self == NULL)
    throw runtime_error("Null pointer");

  free(Self->Buf1);
  free(Self->Buf2);

  Wedling::Secon_Dis(Self->Secon);

  delete Self;
}

void    Window2_Swp(__window2* Self) {

  if (Self == NULL)
    throw runtime_error("Null pointer");

  point Cac  = Self->Buf1;
  Self->Buf1 = Self->Buf2;
  Self->Buf2 = Cac;
}

point   Window2_Buf(__window2* Self) {

  if (Self == NULL)
    throw runtime_error("Null pointer");

  return Self->Buf2;
}

void    Window2_RectF(__window2* Self, rect Rect, color Color) {

  if (Self == NULL)
    throw runtime_error("Null pointer");

  Rect.Right  -= 1;
  Rect.Bottom -= 1;

  // Vert
  for(int32u y = Max(0, Rect.Top); y <= Min(Self->Height -1, Rect.Bottom); y++)
    // Horz
    for (int32u x = Max(0, Rect.Left); x <= min(Self->Width -1, Rect.Right); x++)
      // Fill
      ((int32u*)Self->Buf1)[(Self->Width *y) +x] = Color;

}




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
  ComSet(Window2_Buf);
  ComSet(Window2_RectF);
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
