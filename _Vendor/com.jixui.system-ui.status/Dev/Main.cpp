#include <iostream>
#include <vector>
#include <atomic>
#include <algorithm>
#include <shared_mutex>
#include <mutex>

#include <string.h>

#include "Basis.hpp"
#include "Nucleol.hpp"

using namespace std;
using namespace jix;


#define PopNuc(X)  X = *(s##X*)NucCom.Get(#X);
#define PushNuc(X) NucCom = Com; NucCom.Set(#X, &X); X =

#define Log(X)     logger(__func__, X)
#define Log2(X,Y)  logger(__func__, X, Y)





struct __Icon: qobject
{
  string Text;

  surface2 Icon;
};


// Objs
shared_mutex Icon_Lock;
vector<__Icon*> Icons;


struct __batIcon
{
  u16 ID;
  battery_Info Info;

  surface2 Icon;

  bool Fake = false;
};

u16 BatC = -1;
vector<__batIcon> BatIcon;



inline void Painter_PreBattery()
{
  auto mDrawer = [](const __batIcon Bat)
  {
    Graphic.Surface2.Clear(Bat.Icon);


    Graphic.Surface2.RectF(Bat.Icon, {0,8, 40,20}, color(1,1,1, 1));

    Graphic.Surface2.RectF(Bat.Icon, {0,8, 40.f*(20/100.f) ,20}, color(0.8,0,0, 1));



    string Text;

    if (Bat.Info.Status != battery_Status::bsUnknown)
      Text = "%" +to_string(Bat.Info.Percent);


    size2d TS = Graphic.Surface2.TextSize(Bat.Icon, Text.c_str(), 16);

    Graphic.Surface2.Text(Bat.Icon, Text.c_str(), 16, {(40-TS.W)/2, (36-TS.H)/2 -1}, color(0,0,0));
  };



  // Change ?
  if (u16 C = Battery.Count(); BatC != C)
  {
    BatC = C;


    // Dispose
    for (auto &Bat: BatIcon)
      Bat.Icon->Dis(Bat.Icon);

    

    // New list
    BatIcon.clear();

    if (C == 0)
      BatIcon.push_back({ 0, {}, Nil, true });

    for (u16 i = 0; i < BatC; i++)
      BatIcon.push_back({ i, {}, Nil });
    
  }


  for (auto &Bat: BatIcon)
  {
    if (Bat.Icon == Nil)
      Bat.Icon = Graphic.Surface2.New(40, 36);

    
    if (Bat.Fake)
    {
      Bat.Info = {
        .Percent = 0,
        .Status = battery_Status::bsUnknown,
      };

      mDrawer(Bat);

      continue;
    }

  
    // Change ?
    if (auto I = Battery.Get(Bat.ID); (Bat.Info.Percent != I.Percent) || (Bat.Info.Status != I.Status))
    {
      Bat.Info = I;

      mDrawer(Bat);
    }

  }

}


void Painter(wVisual Sender, surface2 Surface)
{
  Painter_PreBattery();



  // Start
  shared_lock Lock(Icon_Lock);


  size2d Size = Graphic.Surface2.SizeGet(Surface);

  Graphic.Surface2.RectF(Surface, {0,0, Size.W,Size.H}, color(0.4, 0.4, 0.4, 0.5));



  // Icons
  f32 X = Size.W -(40 +10);

  for (auto &A: BatIcon)
  {
    Graphic.Surface2.Draw_Sur2_A(Surface, {X,2}, A.Icon, 0.6);

    X -= 40 +4;
  }

  for (__Icon *&A: Icons)
  {
    Graphic.Surface2.Draw_Sur2(Surface, {X,2}, A->Icon);

    X -= 36 +4;
  }
}


void _Icon_Dis(qobject *Obj)
{
  unique_lock Lock(Icon_Lock);


  auto X = find(Icons.begin(), Icons.end(), (__Icon*)Obj);
  if (X == Icons.end())
    Log2("Icon not found", lFatal);

  
  // If Exists
  Icons.erase(X);

  delete (__Icon*)Obj;
}





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



#pragma region Publish

wPaint _New()
{
  // Interface
  wPaint Ret = Widget.WPaint.New();

  Widget.WVisual.mPaintSet(Ret, &Painter);
  

  // Return
  return Ret;
}


systemUI_Status_Icon _Icon_New(const char* Text, surface2 Icon)
{
  // Interface
  __Icon *Ret = new __Icon();
  Ret->Dis = &_Icon_Dis;


  // Load
  Ret->Text = string(Text);
  Ret->Icon = Icon;


  // Add
  _l_Add: {
    unique_lock Lock(Icon_Lock);

    Icons.push_back(Ret);
  }

  // Return
  return (systemUI_Status_Icon)Ret;
}


#define Self ((__Icon*)__Self)

surface2 _Icon_IconGet(systemUI_Status_Icon __Self)
{
  shared_lock Lock(Icon_Lock);

  return Self->Icon;
}

void _Icon_IconSet(systemUI_Status_Icon __Self, surface2 Icon)
{
  unique_lock Lock(Icon_Lock);

  Self->Icon = Icon;
}


jstring* _Icon_TextGet(systemUI_Status_Icon __Self)
{
  shared_lock Lock(Icon_Lock);

  return JString_New(Self->Text);
}

void _Icon_TextSet(systemUI_Status_Icon __Self, const char* Text)
{
  unique_lock Lock(Icon_Lock);

  Self->Text = string(Text);
}

#undef Self

#pragma endregion






// Global
sNucCom NucCom;


#pragma region NucCom

void Push(sNucCom Com)
{
  PushNuc(SystemUI_Status)
  {
    .New = &_New,

    .Icon = {
      .New = &_Icon_New,

      .IconGet = &_Icon_IconGet,
      .IconSet = &_Icon_IconSet,

      .TextGet = &_Icon_TextGet,
      .TextSet = &_Icon_TextSet,
    }
  };
}

void Pop()
{
  PopNuc(Widget);
  PopNuc(Graphic);
  PopNuc(Battery);
}


extern "C" sNucStd NucStd
{
  .Ver = NucVer,

  .Push = &Push,
  .Pop  = &Pop,
};

#pragma endregion
