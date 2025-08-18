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

#include "HAL/Display.hh"


using namespace std;
using namespace jix;

using namespace hal;
using namespace qaos;


#define Log(X)     kernel::Logger(__func__, X)
#define Log2(X,Y)  kernel::Logger(__func__, X, Y)



// Global
vector<display::sDriver*> Drivers;


display::sHAL HAL = {

  .RegDriver = [](display::sDriver *Driver) -> bool
  {
    // If already exists
    if (find(Drivers.begin(), Drivers.end(), Driver) != Drivers.end())
      return false;

    // Push
    Drivers.push_back(Driver);

    // Log
    Log2(format("RegDriver({}): 0x{:X}", display::Domain, (uPtr)Driver), kernel::lDebug);

    return true;
  },
  
  .DelDriver = [](display::sDriver *Driver) -> bool
  {
    // If not exits
    auto X = find(Drivers.begin(), Drivers.end(), Driver);

    if (X == Drivers.end())
      return false;
    
    // Del
    Drivers.erase(X);

    // Log
    Log2(format("DelDriver({}): 0x{:X}", display::Domain, (uPtr)Driver), kernel::lDebug);

    return true;
  },


  .DRV = {
    .Reset = []()
    {
      for (auto &X: Drivers)
        ((display::sDriver*)X)->Reset();
    },

    .Count = []() -> u32
    {
      u32 Ret = 0;

      for (auto &X: Drivers)
        Ret += ((display::sDriver*)X)->Count();

      return Ret;
    },
  

    // Start/Stop
    .Start = [](u32 Index) -> display::sess
    {
      u32 Offset = 0;

      for (auto &X: Drivers)
      {
        u32 Count = ((display::sDriver*)X)->Count();

        if (Index < Offset +Count)
          return
            {
              .Drv = X,
              .Obj = ((display::sDriver*)X)->Start(Index - Offset)
            };

        Offset += Count;
      }

      return display::sess();
    },

    .Stop = [](display::sess Sess)
    {
      ((display::sDriver*)Sess.Drv)->Stop(Sess.Obj);
    },
  

    .IsValid = [](display::sess Sess) ->  bool
    {
      return ((display::sDriver*)Sess.Drv)->IsValid(Sess.Obj);
    },


    // Modes
    .ModeC = [](display::sess Sess) -> u16
    {
      return ((display::sDriver*)Sess.Drv)->ModeC(Sess.Obj);
    },

    .ModeG = [](display::sess Sess, u16 Mode) -> display::screenMode
    {
      return ((display::sDriver*)Sess.Drv)->ModeG(Sess.Obj, Mode);
    },


    // Connect
    .Connect = [](display::sess Sess, u16 Mode, display::format Format, u8 BufCount) -> bool
    {
      return ((display::sDriver*)Sess.Drv)->Connect(Sess.Obj, Mode, Format, BufCount);
    },

    .DisConnect = [](display::sess Sess)
    {
      ((display::sDriver*)Sess.Drv)->DisConnect(Sess.Obj);
    },

    .IsConnected = [](display::sess Sess) -> bool
    {
      return ((display::sDriver*)Sess.Drv)->IsConnected(Sess.Obj);
    },

    .IsDevConnected = [](display::sess Sess) -> bool
    {
      return ((display::sDriver*)Sess.Drv)->IsDevConnected(Sess.Obj);
    },

    
    // Buffer
    .BufferSize = [](display::sess Sess) -> u32
    {
      return ((display::sDriver*)Sess.Drv)->BufferSize(Sess.Obj);
    },

    .BufferC = [](display::sess Sess) -> u8
    {
      return ((display::sDriver*)Sess.Drv)->BufferC(Sess.Obj);
    },

    .BufferG = [](display::sess Sess, u8 Index) -> point
    {
      return ((display::sDriver*)Sess.Drv)->BufferG(Sess.Obj, Index);
    },


    .BufferStateGet = [](display::sess Sess, u8 Index) -> display::bufstate
    {
      return ((display::sDriver*)Sess.Drv)->BufferStateGet(Sess.Obj, Index);
    },
    
    .BufferStateSet = [](display::sess Sess, u8 Index, display::bufstate State)
    {
      ((display::sDriver*)Sess.Drv)->BufferStateSet(Sess.Obj, Index, State);
    },
    

    .ActiveBuffer = [](display::sess Sess) -> u8
    {
      return ((display::sDriver*)Sess.Drv)->ActiveBuffer(Sess.Obj);
    },

    .JumpBuffer = [](display::sess Sess, u8 Index)
    {
      ((display::sDriver*)Sess.Drv)->JumpBuffer(Sess.Obj, Index);
    },

  },
};



// Nuc Control
extern "C" kernel::nucStd NucStd
{
  .Ver = kernel::NucVer,

  .Push = []()
  {
    kernel::NucReg(display::Domain, &HAL);
  },

};
