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



namespace hal::input
{
  inline const char* Domain = "hal.input";
  

  struct sess
  {
    point Drv;
    point Obj;
  };


  enum shiftStates: u32
  {
    ssLeft     = (u32)1 << 0,
    ssRight    = (u32)1 << 1,
    ssMiddle   = (u32)1 << 2,
    
    ssCtrl     = (u32)1 << 3,
    ssShift    = (u32)1 << 4,
    ssAlt      = (u32)1 << 5,
    ssSuper    = (u32)1 << 6,
    ssAltGr    = (u32)1 << 7,
  };
  using shiftStateSet = u32;

  
  using handlerREL = void (*)(i16 X, i16 Y);
  using handlerWHL = void (*)(i16 W, i16 H);
  using handlerABS = void (*)(u32 X, u32 Y);
  using handlerKEY = void (*)(char *utf8, u32 KeyCode, shiftStateSet State);


  template <typename __S>
  struct gsDriver
  {
    void  (*Reset)();
    u32   (*Count)();

    __S  (*Start)(u32 Index);
    void (*Stop)(__S);

    bool (*IsValid)(__S);

    void (*AddHandlerREL)(__S, handlerREL Handler);
    void (*AddHandlerWHL)(__S, handlerWHL Handler);
    void (*AddHandlerABS)(__S, handlerABS Handler);
    void (*AddHandlerKEY)(__S, handlerKEY Handler);

    void (*DelHandlerREL)(__S, handlerREL Handler);
    void (*DelHandlerWHL)(__S, handlerWHL Handler);
    void (*DelHandlerABS)(__S, handlerABS Handler);
    void (*DelHandlerKEY)(__S, handlerKEY Handler);
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

    gsDriver<input::sess> DRV;
  };

}
