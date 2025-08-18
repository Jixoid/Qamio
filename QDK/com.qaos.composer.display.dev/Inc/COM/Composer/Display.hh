/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#pragma once

#include <iostream>

#include "Basis.h"
#include "Kernel.hh"

#include "HAL/Display.hh"
#include "HAL/Graphic.hh"


using namespace std;
using namespace jix;

using namespace hal;


namespace com::composer::display
{
  inline const char* Domain = "com.composer.display";
  

  using sess = struct {}*;

  struct sDriver
  {
    sess  (*New)(idU UID);
    void  (*Dis)(sess);

    bool  (*AddDisplay)(sess, hal::display::sess);

    u32                (*DisplayC)(sess);
    hal::graphic::sess (*DisplayG)(sess, u32 Index);
    void               (*DisplaySwap)(sess, u32 Index);
  };
  
}
