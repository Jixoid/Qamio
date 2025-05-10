#include <iostream>
#include <vector>
#include <string.h>

#include "Basis.hpp"
#include "Nucleol.hpp"

#include "Widget.hpp"
#include "Standard.hpp"

using namespace std;
using namespace jix;


#define PopNuc(X)  X = *(s##X*)NucCom.Get(#X);
#define PushNuc(X) NucCom = Com; NucCom.Set(#X, &X); X =




// Global
sNucCom NucCom;

void JString_Dis(jstring* Str)
{
  delete Str->Str;

  delete Str;
}

inline jstring* JString_New(string Str)
{
  jstring *Ret = new jstring();
  Ret->Dis = &JString_Dis;
  
  Ret->Str = strdup(Str.c_str());

  return Ret;
}





void Widget_Dis(qobject* Obj)
{
  delete (__widget*)Obj;
}



#pragma region NucCom

void Push(sNucCom Com)
{
  PushNuc(Widget)
  {
    #define Self ((__widget*)__Self)
    .Widget =
    {
      .ParentGet = [](widget __Self)-> widget
      {
        return (widget)Self->Parent();
      },
      .ParentSet = [](widget __Self, widget Parent)
      {
        Self->Parent((__widget*)Parent);
      },
    },
    #undef Self

    #define Self ((__wVisual*)__Self)
    .WVisual =
    {
      .PointGet = [](wVisual __Self)-> point2d
      {
        return Self->Point();
      },
      .PointSet = [](wVisual __Self, point2d Point)
      {
        Self->Point(Point);
      },

      .SizeGet = [](wVisual __Self)-> size2d
      {
        return Self->Size();
      },
      .SizeSet = [](wVisual __Self, size2d Size)
      {
        Self->Size(Size);
      },

      .TextGet = [](wVisual __Self)-> jstring*
      {
        return JString_New(Self->Text());
      },
      .TextSet = [](wVisual __Self, const char* Text)
      {
        Self->Text(string(Text));
      },

      .Canvas = [](wVisual __Self)-> surface2
      {
        return Self->Surface;
      },
      .Draw = [](wVisual __Self)
      {
        Self->DrawBefore();
        Self->Draw();

        if (Self->fmPaint != Nil)
          Self->fmPaint(__Self, Self->Surface);

        Self->DrawAfter();
      },

      .Input_Rel = [](wVisual __Self, i16 X, i16 Y)
      {
        Self->Input_Rel(X,Y);
      },
      
      .mPaintGet = [](wVisual __Self)-> wVisual_mPaint
      {
        return Self->mPaint();
      },
      .mPaintSet = [](wVisual __Self, wVisual_mPaint mPaint)
      {
        Self->mPaint(mPaint);
      },
    },
    #undef Self

    #define Self ((__wPaint*)__Self)
    .WPaint =
    {
      .New = []()-> wPaint
      {
        __wPaint *Ret = new __wPaint();
        Ret->Dis = &Widget_Dis;

        return (wPaint)Ret;
      },
    },
    #undef Self

    #define Self ((__wPanel*)__Self)
    .WPanel =
    {
      .New = []()-> wPanel
      {
        __wPanel *Ret = new __wPanel();
        Ret->Dis = &Widget_Dis;

        return (wPanel)Ret;
      },
    },
    #undef Self

    #define Self ((__wButton*)__Self)
    .WButton =
    {
      .New = []()-> wButton
      {
        __wButton *Ret = new __wButton();
        Ret->Dis = &Widget_Dis;

        return (wButton)Ret;
      },
    },
    #undef Self

    #define Self ((__wLabel*)__Self)
    .WLabel =
    {
      .New = []()-> wLabel
      {
        __wLabel *Ret = new __wLabel();
        Ret->Dis = &Widget_Dis;

        return (wLabel)Ret;
      },
    },
    #undef Self
  };
}

void Pop()
{
  PopNuc(Graphic);
}


extern "C" sNucStd NucStd
{
  .Ver = NucVer,

  .Push = &Push,
  .Pop  = &Pop,
};

#pragma endregion
