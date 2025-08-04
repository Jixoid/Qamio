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

#include "HAL/Audio.hpp"


using namespace std;
using namespace jix;

using namespace hal;
using namespace qaos;


#define Log(X)     kernel::Logger(__func__, X)
#define Log2(X,Y)  kernel::Logger(__func__, X, Y)



// Global
vector<audio::sDriver*> Drivers;


audio::sHAL HAL = {

  .RegDriver = [](audio::sDriver *Driver) -> bool
  {
    // If already exists
    if (find(Drivers.begin(), Drivers.end(), Driver) != Drivers.end())
      return false;

    // Push
    Drivers.push_back(Driver);

    // Log
    Log2(format("RegDriver({}): 0x{:X}", audio::Domain, (uPtr)Driver), kernel::lDebug);

    return true;
  },
  
  .DelDriver = [](audio::sDriver *Driver) -> bool
  {
    // If not exits
    auto X = find(Drivers.begin(), Drivers.end(), Driver);

    if (X == Drivers.end())
      return false;
    
    // Del
    Drivers.erase(X);

    // Log
    Log2(format("DelDriver({}): 0x{:X}", audio::Domain, (uPtr)Driver), kernel::lDebug);

    return true;
  },


  .DRV = {
    .Reset = []()
    {
      for (auto &X: Drivers)
        ((audio::sDriver*)X)->Reset();
    },

    .Count = []() -> u32
    {
      u32 Ret = 0;

      for (auto &X: Drivers)
        Ret += ((audio::sDriver*)X)->Count();

      return Ret;
    },
  

    // Start/Stop
    .Start = [](u32 Index) -> audio::sess
    {
      u32 Offset = 0;

      for (auto &X: Drivers)
      {
        u32 Count = ((audio::sDriver*)X)->Count();

        if (Index < Offset +Count)
          return
            {
              .Drv = X,
              .Obj = ((audio::sDriver*)X)->Start(Index - Offset)
            };

        Offset += Count;
      }

      return audio::sess();
    },

    .Stop = [](audio::sess Sess)
    {
      ((audio::sDriver*)Sess.Drv)->Stop(Sess.Obj);
    },
  

    .IsValid = [](audio::sess Sess) ->  bool
    {
      return ((audio::sDriver*)Sess.Drv)->IsValid(Sess.Obj);
    },


    // Modes
    .ModeC = [](audio::sess Sess) -> u16
    {
      return ((audio::sDriver*)Sess.Drv)->ModeC(Sess.Obj);
    },

    .ModeG = [](audio::sess Sess, u16 Mode) -> audio::audioMode
    {
      return ((audio::sDriver*)Sess.Drv)->ModeG(Sess.Obj, Mode);
    },


    // Connect
    .Connect = [](audio::sess Sess, u16 Mode, u32 Frame, u8 BufCount) -> bool
    {
      return ((audio::sDriver*)Sess.Drv)->Connect(Sess.Obj, Mode, Frame, BufCount);
    },

    .DisConnect = [](audio::sess Sess)
    {
      ((audio::sDriver*)Sess.Drv)->DisConnect(Sess.Obj);
    },

    .IsConnected = [](audio::sess Sess) -> bool
    {
      return ((audio::sDriver*)Sess.Drv)->IsConnected(Sess.Obj);
    },

    .IsDevConnected = [](audio::sess Sess) -> bool
    {
      return ((audio::sDriver*)Sess.Drv)->IsDevConnected(Sess.Obj);
    },

    
    // Buffer
    .BufferSize = [](audio::sess Sess) -> u32
    {
      return ((audio::sDriver*)Sess.Drv)->BufferSize(Sess.Obj);
    },

    .BufferC = [](audio::sess Sess) -> u8
    {
      return ((audio::sDriver*)Sess.Drv)->BufferC(Sess.Obj);
    },

    .BufferG = [](audio::sess Sess, u8 Index) -> point
    {
      return ((audio::sDriver*)Sess.Drv)->BufferG(Sess.Obj, Index);
    },


    .BufferStateGet = [](audio::sess Sess, u8 Index) -> audio::bufstate
    {
      return ((audio::sDriver*)Sess.Drv)->BufferStateGet(Sess.Obj, Index);
    },
    
    .BufferStateSet = [](audio::sess Sess, u8 Index, audio::bufstate State)
    {
      ((audio::sDriver*)Sess.Drv)->BufferStateSet(Sess.Obj, Index, State);
    },
    

    .ActiveBuffer = [](audio::sess Sess) -> u8
    {
      return ((audio::sDriver*)Sess.Drv)->ActiveBuffer(Sess.Obj);
    },

    .JumpBuffer = [](audio::sess Sess, u8 Index)
    {
      ((audio::sDriver*)Sess.Drv)->JumpBuffer(Sess.Obj, Index);
    },

  },
};



// Nuc Control
extern "C" kernel::nucStd NucStd
{
  .Ver = kernel::NucVer,

  .Push = []()
  {
    kernel::NucReg(audio::Domain, &HAL);
  },
  
};
