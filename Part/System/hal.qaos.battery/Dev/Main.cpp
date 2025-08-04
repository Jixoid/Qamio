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

#include "HAL/Battery.hpp"


using namespace std;
using namespace jix;

using namespace hal;
using namespace qaos;


#define Log(X)     kernel::Logger(__func__, X)
#define Log2(X,Y)  kernel::Logger(__func__, X, Y)



// Global
vector<battery::sDriver*> Drivers;


battery::sHAL HAL = {

  .RegDriver = [](battery::sDriver *Driver) -> bool
  {
    // If already exists
    if (find(Drivers.begin(), Drivers.end(), Driver) != Drivers.end())
      return false;

    // Push
    Drivers.push_back(Driver);

    // Log
    Log2(format("RegDriver({}): 0x{:X}", battery::Domain, (uPtr)Driver), kernel::lDebug);

    return true;
  },
  
  .DelDriver = [](battery::sDriver *Driver) -> bool
  {
    // If not exits
    auto X = find(Drivers.begin(), Drivers.end(), Driver);

    if (X == Drivers.end())
      return false;
    
    // Del
    Drivers.erase(X);

    // Log
    Log2(format("DelDriver({}): 0x{:X}", battery::Domain, (uPtr)Driver), kernel::lDebug);

    return true;
  },


  .DRV = {
    .Reset = []()
    {
      for (auto &X: Drivers)
        ((hal::battery::sDriver*)X)->Reset();
    },

    .Count = []() -> u32
    {
      u32 Ret = 0;

      for (auto &X: Drivers)
        Ret += ((hal::battery::sDriver*)X)->Count();

      return Ret;
    },
  
    .Get   = [](u32 Index) -> battery::battery_Info
    {
      u32 Offset = 0;

      for (auto &X: Drivers)
      {
        u32 Count = ((battery::sDriver*)X)->Count();

        if (Index < Offset +Count)
          return ((battery::sDriver*)X)->Get(Index - Offset);

        Offset += Count;
      }


      // Geçersiz index → default nesne döndür, ya da assert/log yap
      return battery::battery_Info{};
    },
  
  },
  
};



// Nuc Control
extern "C" kernel::nucStd NucStd
{
  .Ver = kernel::NucVer,

  .Push = []()
  {
    kernel::NucReg(battery::Domain, &HAL);
  },
  
};
