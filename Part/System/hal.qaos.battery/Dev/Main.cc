/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#include <iostream>
#include <vector>
#include <unordered_map>
#include <bits/stdc++.h>

#include "Basis.h"
#include "Kernel.hh"

#include "HAL/Battery.hh"


using namespace std;
using namespace jix;
using namespace hal;
using namespace qaos;


#define Log(X)     kernel::Logger(__func__, X)
#define Log2(X,Y)  kernel::Logger(__func__, X, Y)



// Global
unordered_map<battery::sDriver*, battery::sinfo*> Drivers;


battery::sHAL HAL = {

  .RegDriver = [](battery::sDriver *Driver, battery::sinfo *Info) -> bool
  {
    // If already exists
    auto X = Drivers.find(Driver);
    
    if (X != Drivers.end())
      return false;


    // Push
    Drivers[Driver] = Info;

    // Log
    #ifdef CONFIG_HAL_battery_debug
      Log2(format("RegDrv: {}", Info->Name), kernel::lDebug);
    #endif

    return true;
  },
  
  .DelDriver = [](battery::sDriver *Driver) -> bool
  {
    // If not exits
    auto X = Drivers.find(Driver);

    if (X == Drivers.end())
      return false;

    // Log
    #ifdef CONFIG_HAL_battery_debug
      Log2(format("DelDrv: {}", X->second->Name), kernel::lDebug);
    #endif
    
    // Del
    Drivers.erase(X);

    return true;
  },


  .DRV = {
    
    .Reset = []()
    {
      for (auto &X: Drivers)
        ((hal::battery::sDriver*)X.first)->Reset();
    },

    .Count = []() -> u32
    {
      u32 Ret = 0;

      for (auto &X: Drivers)
        Ret += ((hal::battery::sDriver*)X.first)->Count();

      return Ret;
    },
  
    
    .Start = [](u32 Index) -> battery::sess
    {
      u32 Offset = 0;

      for (auto &X: Drivers)
      {
        u32 Count = ((battery::sDriver*)X.first)->Count();

        if (Index < Offset +Count)
          return
            {
              .Drv = X.first,
              .Obj = ((battery::sDriver*)X.first)->Start(Index - Offset)
            };

        Offset += Count;
      }

      return battery::sess();
    },

    .Stop = [](battery::sess Sess)
    {
      ((battery::sDriver*)Sess.Drv)->Stop(Sess.Obj);
    },


    .IsValid = [](battery::sess Sess) -> bool
    {
      return ((battery::sDriver*)Sess.Drv)->IsValid(Sess.Obj);
    },


    .Get   = [](battery::sess Sess) -> battery::battery_Info
    {
      return ((battery::sDriver*)Sess.Drv)->Get(Sess.Obj);
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
