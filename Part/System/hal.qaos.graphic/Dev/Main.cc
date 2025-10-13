/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#include <iostream>
#include <vector>
#include <bits/stdc++.h>

#include "Basis.h"
#include "Kernel.hh"

#include "HAL/Graphic.hh"


using namespace std;
using namespace jix;

using namespace hal;
using namespace qaos;


#define Log(X)     kernel::Logger(__func__, X)
#define Log2(X,Y)  kernel::Logger(__func__, X, Y)



// Global
vector<graphic::sDriver*> Drivers;


graphic::sHAL HAL = {

  .RegDriver = [](graphic::sDriver *Driver) -> bool
  {
    // If already exists
    if (find(Drivers.begin(), Drivers.end(), Driver) != Drivers.end())
      return false;

    // Push
    Drivers.push_back(Driver);

    // Log
    Log2(format("RegDriver({}): 0x{:X}", graphic::Domain, (u0)Driver), kernel::lDebug);

    return true;
  },
  
  .DelDriver = [](graphic::sDriver *Driver) -> bool
  {
    // If not exits
    auto X = find(Drivers.begin(), Drivers.end(), Driver);

    if (X == Drivers.end())
      return false;
    
    // Del
    Drivers.erase(X);

    // Log
    Log2(format("DelDriver({}): 0x{:X}", graphic::Domain, (u0)Driver), kernel::lDebug);

    return true;
  },


  .Count = []() -> u32
  {
    return Drivers.size();
  },


  .DRV = {

    // New / Dis
    .New = [](u32 Index, u16 Width, u16 Height) -> graphic::sess
    {
      if (Drivers.size() <= Index)
        return graphic::sess();


      return graphic::sess{
        .Drv = Drivers[Index],
        .Obj = Drivers[Index]->New(0, Width, Height),
      };
    },

    .Dis = [](graphic::sess Sess)
    {
      ((graphic::sDriver*)Sess.Drv)->Dis(Sess.Obj);
    },


    // New With
    .New_With_Mem = [](u32 Index, u16 Width, u16 Height, point Buffer) -> graphic::sess
    {
      if (Drivers.size() <= Index)
        return graphic::sess();


      return graphic::sess{
        .Drv = Drivers[Index],
        .Obj = Drivers[Index]->New_With_Mem(0, Width, Height, Buffer),
      };
    },

    .New_With_Sur = [](u32 Index, graphic::sess Sample) -> graphic::sess
    {
      if (Drivers.size() <= Index && Drivers[Index] != Sample.Drv)
        return graphic::sess();


      return graphic::sess{
        .Drv = Drivers[Index],
        .Obj = Drivers[Index]->New_With_Sur(0, Sample.Obj),
      };
    },

    .New_With_Png = [](u32 Index, const char* Path) -> graphic::sess
    {
      if (Drivers.size() <= Index)
        return graphic::sess();


      return graphic::sess{
        .Drv = Drivers[Index],
        .Obj = Drivers[Index]->New_With_Png(0, Path),
      };
    },

    .New_With_Bmp = [](u32 Index, const char* Path) -> graphic::sess
    {
      if (Drivers.size() <= Index)
        return graphic::sess();


      return graphic::sess{
        .Drv = Drivers[Index],
        .Obj = Drivers[Index]->New_With_Bmp(0, Path),
      };
    },


    // Pre Config
    .Set_Color = [](graphic::sess Sess, graphic::color Color)
    {
      ((graphic::sDriver*)Sess.Drv)->Set_Color(Sess.Obj, Color);
    },

    .Set_Source = [](graphic::sess Sess, graphic::sess Src)
    {
      if (Sess.Drv != Src.Drv)
        return;

      ((graphic::sDriver*)Sess.Drv)->Set_Source(Sess.Obj, Src.Obj);
    },

    .Set_FontSize = [](graphic::sess Sess, f32 Size)
    {
      ((graphic::sDriver*)Sess.Drv)->Set_FontSize(Sess.Obj, Size);
    },

    .Set_LineSize = [](graphic::sess Sess, f32 Size)
    {
      ((graphic::sDriver*)Sess.Drv)->Set_LineSize(Sess.Obj, Size);
    },


    // Func
    .Get_Size = [](graphic::sess Sess, graphic::size2_i32 *Ret)
    {
      return ((graphic::sDriver*)Sess.Drv)->Get_Size(Sess.Obj, Ret);
    },

    .Set_Size = [](graphic::sess Sess, u16 Width, u16 Height)
    {
      ((graphic::sDriver*)Sess.Drv)->Set_Size(Sess.Obj, Width, Height);
    },

    .Set_Pos = [](graphic::sess Sess, graphic::poit2_f32 Val)
    {
      ((graphic::sDriver*)Sess.Drv)->Set_Pos(Sess.Obj, Val);
    },

    .Set_rPos = [](graphic::sess Sess, graphic::poit2_f32 Val)
    {
      ((graphic::sDriver*)Sess.Drv)->Set_rPos(Sess.Obj, Val);
    },


    // Basis
    .Draw_Rect = [](graphic::sess Sess, graphic::rect2_f32 Rect)
    {
      ((graphic::sDriver*)Sess.Drv)->Draw_Rect(Sess.Obj, Rect);
    },

    .Draw_RectRound = [](graphic::sess Sess, graphic::rect2_f32 Rect, f32 Radius)
    {
      ((graphic::sDriver*)Sess.Drv)->Draw_RectRound(Sess.Obj, Rect, Radius);
    },

    .Draw_RectRound4 = [](graphic::sess Sess, graphic::rect2_f32 Rect, f32 R_LT, f32 R_LB, f32 R_RT, f32 R_RB)
    {
      ((graphic::sDriver*)Sess.Drv)->Draw_RectRound4(Sess.Obj, Rect, R_LT, R_LB, R_RT, R_RB);
    },

    .Draw_Line = [](graphic::sess Sess, graphic::poit2_f32 P1, graphic::poit2_f32 P2)
    {
      ((graphic::sDriver*)Sess.Drv)->Draw_Line(Sess.Obj, P1,P2);
    },

    .Draw_ToLine = [](graphic::sess Sess, graphic::poit2_f32 P)
    {
      ((graphic::sDriver*)Sess.Drv)->Draw_ToLine(Sess.Obj, P);
    },

    .Draw_Arc = [](graphic::sess Sess, graphic::poit2_f32 Point, f32 Round, f32 Angle1, f32 Angle2)
    {
      ((graphic::sDriver*)Sess.Drv)->Draw_Arc(Sess.Obj, Point, Round, Angle1, Angle2);
    },

    .Draw_Text = [](graphic::sess Sess, const char* Text)
    {
      ((graphic::sDriver*)Sess.Drv)->Draw_Text(Sess.Obj, Text);
    },

    .Calc_Text = [](graphic::sess Sess, const char* Text, graphic::size2_f32 *Ret)
    {
      return ((graphic::sDriver*)Sess.Drv)->Calc_Text(Sess.Obj, Text, Ret);
    },


    // API
    .Stroke = [](graphic::sess Sess)
    {
      ((graphic::sDriver*)Sess.Drv)->Stroke(Sess.Obj);
    },

    .Fill = [](graphic::sess Sess)
    {
      ((graphic::sDriver*)Sess.Drv)->Fill(Sess.Obj);
    },

    .Paint = [](graphic::sess Sess)
    {
      ((graphic::sDriver*)Sess.Drv)->Paint(Sess.Obj);
    },

    .Paint_A = [](graphic::sess Sess, f32 Alpha)
    {
      ((graphic::sDriver*)Sess.Drv)->Paint_A(Sess.Obj, Alpha);
    },

    .Clip = [](graphic::sess Sess)
    {
      ((graphic::sDriver*)Sess.Drv)->Clip(Sess.Obj);
    },

    .Clip_Reset = [](graphic::sess Sess)
    {
      ((graphic::sDriver*)Sess.Drv)->Clip_Reset(Sess.Obj);
    },


    .Pixel = [](graphic::sess Sess, graphic::poit2_i32 Point) -> graphic::color
    {
      return ((graphic::sDriver*)Sess.Drv)->Pixel(Sess.Obj, Point);
    },

    .Clear = [](graphic::sess Sess)
    {
      ((graphic::sDriver*)Sess.Drv)->Clear(Sess.Obj);
    },

    .Flush = [](graphic::sess Sess)
    {
      ((graphic::sDriver*)Sess.Drv)->Flush(Sess.Obj);
    },

  },
};



// Nuc Control
extern "C" kernel::nucStd NucStd
{
  .Ver = kernel::NucVer,

  .Push = []()
  {
    kernel::NucReg(graphic::Domain, &HAL);
  },

};
