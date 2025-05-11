#include <iostream>
#include <vector>
#include <mutex>
#include <bits/stdc++.h>

#include "Basis.hpp"
#include "Nucleol.hpp"

#include "Widget.hpp"

using namespace std;
using namespace jix;


#define Log(X)     logger(__func__, X)
#define Log2(X,Y)  logger(__func__, X, Y)



#pragma region __widget

// API
inline void __ul_Parent(__widget *Self, __widget *Val)
{
  // Optimize
  if (Self->fParent == Val)
    return;


  // Clear last
  if (Self->fParent != Nil)
  {
    unique_lock PLock(Self->fParent->Locker);

    Self->fParent->Childs.erase(find(Self->fParent->Childs.begin(), Self->fParent->Childs.end(), Self));
  }


  // Add new
  if (Val != Nil)
  {
    unique_lock PLock(Val->Locker);

    Val->Childs.push_back(Self);
  }

  Self->fParent = Val;
};



// Basic
__widget::__widget()
{
  unique_lock Lock(Locker);

  __New();
}

__widget::~__widget()
{    
  unique_lock Lock(Locker);

  __Dis();
}


void __widget::__New() {}

void __widget::__Dis()
{
  __ul_Parent(this, Nil);
}



// Property
__widget* __widget::Parent()
{
  shared_lock Lock(Locker);

  return fParent;
}

void __widget::Parent(__widget *Val)
{
  unique_lock Lock(Locker);

  __ul_Parent(this, Val);
};

#pragma endregion



#pragma region __wVisual

// API
inline void __Parent_MustDraw(__wVisual *Self)
{
  if (Self->fParent == Nil)
    return;


  __wVisual* Visual = dynamic_cast<__wVisual*>(Self->fParent);
  if (Visual != Nil) {
    unique_lock PLock(Visual->Locker);

    Visual->fMust_Draw = true;
  }
}

inline void __ul_Optimize_Location(__wVisual *Self)
{
  Self->fEnd = {
    .X = Self->fPoint.X +Self->fSize.W,
    .Y = Self->fPoint.Y +Self->fSize.H, 
  };
}



// Basic
__wVisual::__wVisual()
{
  unique_lock Lock(Locker);

  __New();
}

__wVisual::~__wVisual()
{
  unique_lock Lock(Locker);

  __Dis();
}


void __wVisual::__New()
{
  __widget::__New();

  // Canvas
  Canvas = Graphic.Window.New(fSize.W, fSize.H);
}

void __wVisual::__Dis()
{
  // Canvas
  Canvas->Dis(Canvas);
  Canvas = Nil;
  Surface = Nil;

  __widget::__Dis();
}



// Other
void __wVisual::DrawBefore()
{
  unique_lock Lock(Locker);

  Surface = Graphic.Window.Sur(Canvas);
  
  Graphic.Surface2.Clear(Surface);
}

void __wVisual::DrawAfter()
{
  //Log("Paint");

  unique_lock Lock(Locker);

  for (auto it = Childs.rbegin(); it != Childs.rend(); ++it)
  {
    const auto &Child = *it;

    // Check
    __wVisual *Visual = dynamic_cast<__wVisual*>(Child);
    if (Visual == Nil)
      continue;


    // Draw
    if (Visual->fMust_Draw)
    {
      Visual->DrawBefore();
      Visual->Draw();

      if (Visual->fmPaint != Nil)
        Visual->fmPaint(Visual, Visual->Surface);

      Visual->DrawAfter();
    }

    // Transfer
    Graphic.Surface2.Draw_Sur2(Surface, Visual->fPoint, Visual->Surface);
  }

  Graphic.Window.Swap(Canvas);
  fMust_Draw = false;
}

void __wVisual::Draw()
{  
  unique_lock Lock(Locker);

  // Your draws...
}


void __wVisual::Input_Abs(f32 X, f32 Y)
{
  __wVisual *Target = Nil;
  f32 MeX, MeY;
  bool Changed;


  _l_Find: {
    shared_lock Lock(Locker);

    // Optimization
    MeX = fPoint.X;
    MeY = fPoint.Y;


    for (const auto &Child: Childs)
    {
      // Check
      __wVisual *Visual = dynamic_cast<__wVisual*>(Child);
      if (Visual == Nil)
        continue;

      
      if (Target != Nil)
        goto _l_Focus_OFF;
      
      
      // Check
      _l_Check_Child: {
        shared_lock CLock(Visual->Locker);

        //cout << "V: " << (intPu)Visual << ", X: " << X << ", PX: " << Visual->fPoint.X << endl;

        if (!(X > Visual->fPoint.X && Y > Visual->fPoint.Y))
          goto _l_Focus_OFF;

        if (!(X < Visual->fEnd.X && Y < Visual->fEnd.Y))
          goto _l_Focus_OFF;


        Target = Visual;
        continue;
      }


      _l_Focus_OFF: {
        
        unique_lock CLock(Visual->Locker);

        if (Visual->fFocus != false) {
          Visual->fFocus = false;

          Visual->fMust_Draw = true;
        }
      }
    }


    Changed = (Target != fFocusCtrl);
    fFocusCtrl = Target;

    if (Target == Nil)
      Target = this;

  } // Find


  if (!Changed)
    goto _l_Call_Child;


  _l_Focus_ON: {
    unique_lock Lock(Target->Locker);

    Target->fFocus = true;

    Target->fMust_Draw = true;
  }


  _l_DrawSelf: {
    unique_lock Lock(Locker);

    fFocus = (Target == this);

    fMust_Draw = true;
  }


  _l_Call_Child: {
    if (Target != this)
      Target->Input_Abs(X -MeX, Y -MeY);
  }

}



// Property
point2d __wVisual::Point()
{
  shared_lock Lock(Locker);

  return fPoint;
}

void __wVisual::Point(point2d Val)
{
  _l_Set: {
  unique_lock Lock(Locker);

  fPoint = Val;


  __ul_Optimize_Location(this);
  }

  __Parent_MustDraw(this);
};

size2d __wVisual::Size()
{
  shared_lock Lock(Locker);

  return fSize;
}

void __wVisual::Size(size2d Val)
{

  _l_Set: {
  unique_lock Lock(Locker);

  Graphic.Window.SizeSet(Canvas, Val);

  fSize = Val;


  __ul_Optimize_Location(this);
  }

  __Parent_MustDraw(this);
};


string __wVisual::Text()
{  
  shared_lock Lock(Locker);

  return fText;
}

void __wVisual::Text(string Val)
{

  _l_Set: {
  unique_lock Lock(Locker);

  fText = Val;
  }

  __Parent_MustDraw(this);
};


wVisual_mPaint __wVisual::mPaint()
{
  shared_lock Lock(Locker);

  return fmPaint;
}

void __wVisual::mPaint(wVisual_mPaint Val)
{

  _l_Set: {
    unique_lock Lock(Locker);

    fmPaint = Val;
  }

  __Parent_MustDraw(this);
}

#pragma endregion

