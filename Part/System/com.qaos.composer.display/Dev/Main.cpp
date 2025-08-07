/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include <unordered_map>
#include <wait.h>

#include "Basis.h"
#include "Kernel.hpp"

#include "COM/Composer/Display.hpp"

#include "HAL/Display.hpp"
#include "HAL/Graphic.hpp"


using namespace std;
using namespace jix;

using namespace qaos;
using namespace com;
using namespace hal;

#define Log(X)     kernel::Logger(__func__, X)
#define Log2(X,Y)  kernel::Logger(__func__, X, Y)



// Priv
struct __sess
{
  struct __display
  {
    hal::display::sess DispSess;
    hal::graphic::sess Buf1, Buf2;
    u8 BufIndex;
  };

  vector<__display> Display;
};

hal::display::sHAL *Display;
hal::graphic::sHAL *Graphic;



// Global
composer::display::sDriver DRV = {

  #define Self ((__sess*)__Self)

  .New = [](idU UID) -> composer::display::sess
  {
    // New
    __sess *Ret = new __sess();
    

    // Ret
    return (composer::display::sess)Ret;
  },

  .Dis = [](composer::display::sess __Self)
  {
    // Dis
    delete Self;
  },


  .AddDisplay = [](composer::display::sess __Self, hal::display::sess DispSess) -> bool
  {
    // Graphic
    if (Graphic->Count() == 0)
      return false;

    // Adapter
    if (Display->DRV.IsConnected(DispSess))
      return false;

    if (! Display->DRV.Connect(DispSess, 0, hal::display::format::iXRGB_8888, 2))
      return false;
  

    auto Mode0 = Display->DRV.ModeG(DispSess, 0);

    Self->Display.push_back({
      .DispSess = DispSess,

      .Buf1 = Graphic->DRV.New_With_Mem(0,
        Mode0.Width,
        Mode0.Height,
        Display->DRV.BufferG(DispSess, 0)
      ),

      .Buf2 = Graphic->DRV.New_With_Mem(0,
        Mode0.Width,
        Mode0.Height,
        Display->DRV.BufferG(DispSess, 1)
      ),

      .BufIndex = 0,
    });

    Display->DRV.JumpBuffer(DispSess, 0);
    

    return true;
  },


  .DisplayC = [](composer::display::sess __Self) -> u32
  {
    return Self->Display.size();
  },

  .DisplayG = [](composer::display::sess __Self, u32 Index) -> hal::graphic::sess
  {
    return Self->Display[Index].Buf2;
  },

  .DisplaySwap = [](composer::display::sess __Self, u32 Index)
  {
    auto Cac = Self->Display[Index].Buf1;
    Self->Display[Index].Buf1 = Self->Display[Index].Buf2;
    Self->Display[Index].Buf2 = Cac;

    Self->Display[Index].BufIndex = (Self->Display[Index].BufIndex == 0) ? 1:0;

    Display->DRV.JumpBuffer(Self->Display[Index].DispSess, Self->Display[Index].BufIndex);
  }

  #undef Self

};



// Nuc Control
extern "C" kernel::nucStd NucStd
{
  .Ver = kernel::NucVer,

  .Push = []()
  {
    kernel::NucReg(composer::display::Domain, &DRV);
  },

  .Pop = []()
  {
    Display = (decltype(Display))kernel::NucGet(hal::display::Domain);
    if (Display == Nil)
      Log2("Depency: "+ string(hal::display::Domain) +" is not found", kernel::lPanic);

    Graphic = (decltype(Graphic))kernel::NucGet(hal::graphic::Domain);
    if (Graphic == Nil)
      Log2("Depency: "+ string(hal::graphic::Domain) +" is not found", kernel::lPanic);
    
  },

};
