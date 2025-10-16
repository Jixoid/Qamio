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


using namespace std;
using namespace jix;



namespace hal::battery
{
  inline const char* Domain = "hal.battery";
  

  struct sess
  {
    point Drv;
    point Obj;
  };

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




  template <typename __S>
  struct gsDriver
  {
    void  (*Reset)();
    u32   (*Count)();

    __S  (*Start)(u32 Index);
    void (*Stop)(__S);

    bool (*IsValid)(__S);

    battery_Info (*Get)(__S);
  };

  using sDriver = gsDriver<point>;

  struct sinfo
  {
    const char *Name = Nil;
  };


  struct sHAL
  {
    bool  (*RegDriver)(sDriver *Driver, sinfo *Info);
    bool  (*DelDriver)(sDriver *Driver);

    gsDriver<battery::sess> DRV;
  };

}
