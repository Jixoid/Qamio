#include <iostream>
#include <vector>
#include <bits/stdc++.h>

#include "Nucleol.hpp"
#include "Graphic.hpp"
#include "Widget.hpp"

using namespace std;
using namespace jix;




// Widget_Wedling

class __widget {
public:
  // Widget
  __widget* fParent = Nil;
  vector<__widget*> Childrens;

  
public:
  // Basic
  explicit __widget(__widget* nParent = Nil) {

    Parent(nParent);
  }

  virtual ~__widget() {
    
    Parent(Nil);
  }


public:
  // Property
  __widget* Parent() {
    return fParent;
  }

  void Parent(__widget* Val) {
    // Clear last
    if (fParent != Nil)
      fParent->Childrens.erase(find(fParent->Childrens.begin(), fParent->Childrens.end(), this));
    

    // Add new
    if (Val != Nil)
      Val->Childrens.push_back(this);

    fParent = Val;
  };

};


class __wvisual: public __widget {
protected:
  // Location
  point2d fPoint = {0,0};
  size2d  fSize = {100,100};

  // Visual State
  enum eState: int8u {
    DrawRequir,
    Drawed,
  } VState = eState::DrawRequir;


public:
  // GUI
  window2  Canvas = Nil;
  surface2 Surface = Nil;


public:
  // Basic
  explicit __wvisual(__widget* nParent = Nil): __widget(nParent) {
    
    Canvas = Graphic::Window2_New(fSize.Width, fSize.Height);
  }

  ~__wvisual () override {
    
    Graphic::Window2_Dis(Canvas);
  }


public:
  // Other
  virtual void DrawBefore() {
    Surface = Graphic::Window2_Sur(Canvas);
  }

  virtual void DrawAfter() {

    for (auto &Child: Childrens) {
      // Check
      __wvisual* Visual = dynamic_cast<__wvisual*>(Child);
      if (Visual == Nil)
        continue;


      // Draw
      if (Visual->VState == __wvisual::eState::DrawRequir) {
        Visual->DrawBefore();
        Visual->Draw();
        Visual->DrawAfter();
      }

      // Transfer
      Graphic::Surface2_Draw_Sur2(Surface, Visual->Surface, Visual->fPoint);
    }

    Graphic::Window2_Swp(Canvas);
    VState = eState::Drawed;
  }

  virtual void  Draw() {
    
  }


public:
  // Property
  point2d Point() {
    return fPoint;
  }

  void Point(point2d Val) {
    fPoint = Val;
  };


  size2d Size() {
    return fSize;
  }

  void Size(size2d Val) {
    Graphic::Window2_SizeSet(Canvas, Val);

    fSize = Val;
  };

};


class __wpanel: public __wvisual {
public:


public:
  // Basic
  explicit __wpanel(__widget* nParent = Nil): __wvisual(nParent) {
    
  }

  ~__wpanel () override {
  }


public:
  // Other
  void  Draw() override {

    Graphic::Surface2_RectF(Surface,
      {
        .L = 0, .T = 0,
        .W = fSize.Width,
        .H = fSize.Height,
      },
      color(0.1, 0.1, 0.1, 1)
    );

  }

};


class __wbutton: public __wvisual {
public:


public:
  // Basic
  explicit __wbutton(__widget* nParent = Nil): __wvisual(nParent) {
    
  }

  ~__wbutton () override {
  }


public:
  // Other
  void  Draw() override {

    Graphic::Surface2_RectF(Surface,
      {
        .L = 0, .T = 0,
        .W = fSize.Width,
        .H = fSize.Height,
      },
      color(0.3, 0.3, 0.3, 1)
    );

  }

};
