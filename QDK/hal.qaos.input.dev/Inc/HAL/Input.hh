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

  
  using inputHandlerREL = void (*)(i16 X, i16 Y);
  using inputHandlerWHL = void (*)(i16 W, i16 H);
  using inputHandlerABS = void (*)(u32 X, u32 Y);


  template <typename __S>
  struct gsDriver
  {
    void  (*Reset)();
    u32   (*Count)();

    __S  (*Start)(u32 Index);
    void (*Stop)(__S);

    bool (*IsValid)(__S);

    void (*AddHandlerREL)(__S, inputHandlerREL Handler);
    void (*AddHandlerWHL)(__S, inputHandlerWHL Handler);
    void (*AddHandlerABS)(__S, inputHandlerABS Handler);

    void (*DelHandlerREL)(__S, inputHandlerREL Handler);
    void (*DelHandlerWHL)(__S, inputHandlerWHL Handler);
    void (*DelHandlerABS)(__S, inputHandlerABS Handler);
  };

  using sDriver = gsDriver<point>;


  struct sHAL
  {
    bool  (*RegDriver)(sDriver *Driver);
    bool  (*DelDriver)(sDriver *Driver);

    gsDriver<input::sess> DRV;
  };

}
