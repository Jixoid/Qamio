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
#include "Kernel.hpp"

#include "HAL/Graphic.hpp"


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
    Log2(format("RegDriver({}): 0x{:X}", graphic::Domain, (uPtr)Driver), kernel::lDebug);

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
    Log2(format("DelDriver({}): 0x{:X}", graphic::Domain, (uPtr)Driver), kernel::lDebug);

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


    // API
    .Pixel = [](graphic::sess Sess, graphic::point2d Point) -> graphic::color
    {
      return ((graphic::sDriver*)Sess.Drv)->Pixel(Sess.Obj, Point);
    },

    .Clear = [](graphic::sess Sess)
    {
      ((graphic::sDriver*)Sess.Drv)->Clear(Sess.Obj);
    },


    // Size
    .SizeGet = [](graphic::sess Sess) -> graphic::size2d
    {
      return ((graphic::sDriver*)Sess.Drv)->SizeGet(Sess.Obj);
    },

    .SizeSet = [](graphic::sess Sess, u16 Width, u16 Height)
    {
      ((graphic::sDriver*)Sess.Drv)->SizeSet(Sess.Obj, Width, Height);
    },


    // Pre Config
    .Set_Source_RGB = [](graphic::sess Sess, graphic::color Color)
    {
      ((graphic::sDriver*)Sess.Drv)->Set_Source_RGB(Sess.Obj, Color);
    },

    .Set_Source_RGBA = [](graphic::sess Sess, graphic::color Color)
    {
      ((graphic::sDriver*)Sess.Drv)->Set_Source_RGBA(Sess.Obj, Color);
    },

    .Set_Source_Sur = [](graphic::sess Sess, graphic::sess Src, graphic::point2d Point)
    {
      if (Sess.Drv != Src.Drv)
        return;

      ((graphic::sDriver*)Sess.Drv)->Set_Source_Sur(Sess.Obj, Src.Obj, Point);
    },

    .Set_Font_Size = [](graphic::sess Sess, f32 Size)
    {
      ((graphic::sDriver*)Sess.Drv)->Set_Font_Size(Sess.Obj, Size);
    },


    // Basis
    .Basis_Move = [](graphic::sess Sess, graphic::point2d Point)
    {
      ((graphic::sDriver*)Sess.Drv)->Basis_Move(Sess.Obj, Point);
    },


    .Basis_Line = [](graphic::sess Sess, graphic::point2d Point)
    {
      ((graphic::sDriver*)Sess.Drv)->Basis_Line(Sess.Obj, Point);
    },

    .Basis_Rect = [](graphic::sess Sess, graphic::rect2d Rect)
    {
      ((graphic::sDriver*)Sess.Drv)->Basis_Rect(Sess.Obj, Rect);
    },

    .Basis_Arc = [](graphic::sess Sess, graphic::point2d Point, f32 Round, f32 Angle1, f32 Angle2)
    {
      ((graphic::sDriver*)Sess.Drv)->Basis_Arc(Sess.Obj, Point, Round, Angle1, Angle2);
    },


    .Basis_RectR = [](graphic::sess Sess, graphic::rect2d Rect, f32 Round)
    {
      ((graphic::sDriver*)Sess.Drv)->Basis_RectR(Sess.Obj, Rect, Round);
    },


    // Text
    .Text_Size = [](graphic::sess Sess, const char* Text) -> graphic::size2d
    {
      return ((graphic::sDriver*)Sess.Drv)->Text_Size(Sess.Obj, Text);
    },

    .Text = [](graphic::sess Sess, const char* Text)
    {
      ((graphic::sDriver*)Sess.Drv)->Text(Sess.Obj, Text);
    },


    // OK
    .Flush = [](graphic::sess Sess)
    {
      ((graphic::sDriver*)Sess.Drv)->Flush(Sess.Obj);
    },

    .Paint = [](graphic::sess Sess)
    {
      ((graphic::sDriver*)Sess.Drv)->Paint(Sess.Obj);
    },

    .Paint_A = [](graphic::sess Sess, f32 Alpha)
    {
      ((graphic::sDriver*)Sess.Drv)->Paint_A(Sess.Obj, Alpha);
    },

    .Fill = [](graphic::sess Sess)
    {
      ((graphic::sDriver*)Sess.Drv)->Fill(Sess.Obj);
    },

    .Stroke = [](graphic::sess Sess)
    {
      ((graphic::sDriver*)Sess.Drv)->Stroke(Sess.Obj);
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
