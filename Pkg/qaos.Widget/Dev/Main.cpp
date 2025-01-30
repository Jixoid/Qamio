#include <iostream>
#include <vector>

#include "Nucleol.hpp"
#include "Graphic.hpp"
#include "Widget.hpp"

#include "Widget.cpp"

using namespace std;
using namespace jix;


#define ComSet(X) NucCom.Set(#X, (point)X);




#pragma region Publish

// Widget
void     Widget_Widget_Dis(__widget* Self) {

  if (Self == NULL)
    throw runtime_error("Null pointer");


  delete Self;
}


// Visual
point2d  Widget_Visual_PointGet(__wvisual* Self) {
  
  if (Self == NULL)
    throw runtime_error("Null pointer");

  
  return Self->Point();
}

void     Widget_Visual_PointSet(__wvisual* Self, point2d Point) {

  if (Self == NULL)
    throw runtime_error("Null pointer");

  
  Self->Point(Point);
}

size2d   Widget_Visual_SizeGet(__wvisual* Self) {
  
  if (Self == NULL)
    throw runtime_error("Null pointer");

  
  return Self->Size();
}

void     Widget_Visual_SizeSet(__wvisual* Self, size2d Size) {

  if (Self == NULL)
    throw runtime_error("Null pointer");

  
  Self->Size(Size);
}

surface2 Widget_Visual_Buf(__wvisual* Self) {

  if (Self == NULL)
    throw runtime_error("Null pointer");


  return Graphic::Window2_Buf(Self->Canvas);
}

void     Widget_Visual_Draw(__wvisual* Self) {
  
  if (Self == NULL)
    throw runtime_error("Null pointer");


  Self->DrawBefore();
  Self->Draw();
  Self->DrawAfter();
}


// Button
wbutton   Widget_Button_New(__widget* Parent) {

  __wbutton* Self = new __wbutton(Parent);

  return (point)Self;
}


// Panel
wpanel   Widget_Panel_New(__widget* Parent) {

  __wpanel* Self = new __wpanel(Parent);

  return (point)Self;
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

  // Widget
  ComSet(Widget_Widget_Dis);


  // Visual
  ComSet(Widget_Visual_PointGet);
  ComSet(Widget_Visual_PointSet);
  ComSet(Widget_Visual_SizeGet);
  ComSet(Widget_Visual_SizeSet);
  ComSet(Widget_Visual_Buf);
  ComSet(Widget_Visual_Draw);


  // Button
  ComSet(Widget_Button_New);

  // Panel
  ComSet(Widget_Panel_New);
}

extern "C" void NucPop() {

  Graphic::PopNuc(NucCom);
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
