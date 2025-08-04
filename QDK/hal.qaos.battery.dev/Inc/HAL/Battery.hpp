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
#include "Kernel.hpp"


using namespace std;
using namespace jix;



namespace hal::battery
{
  inline const char* Domain = "hal.battery";
  
  enum battery_Status: u8
  {
    bsUnknown     = 0,
    bsCharging    = 1,
    bsDisCharging = 2,
    bsNotCharging = 3,
    bsFull        = 4,
  };

  struct battery_Info
  {
    u8 Percent;
    battery_Status Status;
  };




  struct sDriver
  {
    void  (*Reset)();
    u32   (*Count)();

    battery_Info (*Get)(u32 Index);
  };

  struct sHAL
  {
    bool  (*RegDriver)(sDriver *Driver);
    bool  (*DelDriver)(sDriver *Driver);

    sDriver DRV;
  };

}
