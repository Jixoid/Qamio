#pragma once

#include <iostream>
#include <vector>
#include <mutex>
#include <shared_mutex>

#include "Basis.hpp"
#include "Nucleol.hpp"

using namespace std;
using namespace jix;



class __widget: public qobject {
public:
  // Widget
  __widget *fParent = Nil;
  vector<__widget*> Childs;


  // Mutex, Thread locker
  shared_mutex Locker;

  
public:
  // Basic
   __widget();
  ~__widget();


  virtual void __New();
  virtual void __Dis();


public:
  // Property
  __widget* Parent();
  void Parent(__widget *Val);

};


class __wVisual: public __widget {
public:
  // Location
  point2d fPoint = {0,0};
  size2d  fSize  = {100,100};
  point2d fEnd   = {100,100};  // For optimization


  // Props
  string fText;

  __wVisual *fFocusCtrl = Nil;
  

  // Visual States
  bool fMust_Draw = true;

  bool fFocus = false;


public:
  // GUI
  window   Canvas = Nil;
  surface2 Surface = Nil;


public:
  // Basic
   __wVisual();
  ~__wVisual();


  void __New() override;
  void __Dis() override;

public:
  // Other
  void DrawBefore();
  void DrawAfter();

  virtual void Draw();


  void Input_Rel(f32 X, f32 Y);


public:
  // Property
  point2d Point();
  void Point(point2d Val);

  size2d Size();
  void Size(size2d Val);
  
  string Text();
  void Text(string Val);

};
