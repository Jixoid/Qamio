/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#include <iostream>
#include <vector>
#include <atomic>
#include <bits/stdc++.h>

#include "Basis.h"
#include "Kernel.hh"

#include "HAL/Input.hh"


using namespace std;
using namespace jix;

using namespace hal;
using namespace qaos;


#define Log(X)     kernel::Logger(__func__, X)
#define Log2(X,Y)  kernel::Logger(__func__, X, Y)



// Global
vector<input::sDriver*> Drivers;



input::sHAL HAL = {

  .RegDriver = [](input::sDriver *Driver) -> bool
  {
    // If already exists
    if (find(Drivers.begin(), Drivers.end(), Driver) != Drivers.end())
      return false;

    // Push
    Drivers.push_back(Driver);

    // Log
    Log2(format("RegDriver({}): 0x{:X}", input::Domain, (u0)Driver), kernel::lDebug);

    return true;
  },
  
  .DelDriver = [](input::sDriver *Driver) -> bool
  {
    // If not exits
    auto X = find(Drivers.begin(), Drivers.end(), Driver);

    if (X == Drivers.end())
      return false;
    
    // Del
    Drivers.erase(X);

    // Log
    Log2(format("DelDriver({}): 0x{:X}", input::Domain, (u0)Driver), kernel::lDebug);

    return true;
  },


  .DRV = {

    .Reset = []()
    {
      for (auto &X: Drivers)
        ((hal::input::sDriver*)X)->Reset();
    },

    .Count = []() -> u32
    {
      u32 Ret = 0;

      for (auto &X: Drivers)
        Ret += ((hal::input::sDriver*)X)->Count();

      return Ret;
    },
  

    .Start = [](u32 Index) -> input::sess
    {
      u32 Offset = 0;

      for (auto &X: Drivers)
      {
        u32 Count = ((input::sDriver*)X)->Count();

        if (Index < Offset +Count)
          return
            {
              .Drv = X,
              .Obj = ((input::sDriver*)X)->Start(Index - Offset)
            };

        Offset += Count;
      }

      return input::sess();
    },

    .Stop = [](input::sess Sess)
    {
      ((input::sDriver*)Sess.Drv)->Stop(Sess.Obj);
    },


    .IsValid = [](input::sess Sess) -> bool
    {
      return ((input::sDriver*)Sess.Drv)->IsValid(Sess.Obj);
    },


    .AddHandlerREL = [](input::sess Sess, input::handlerREL Handler)
    {
      ((input::sDriver*)Sess.Drv)->AddHandlerREL(Sess.Obj, Handler);
    },
    
    .AddHandlerWHL = [](input::sess Sess, input::handlerWHL Handler)
    {
      ((input::sDriver*)Sess.Drv)->AddHandlerWHL(Sess.Obj, Handler);
    },

    .AddHandlerABS = [](input::sess Sess, input::handlerABS Handler)
    {
      ((input::sDriver*)Sess.Drv)->AddHandlerABS(Sess.Obj, Handler);
    },

    .AddHandlerKEY = [](input::sess Sess, input::handlerKEY Handler)
    {
      ((input::sDriver*)Sess.Drv)->AddHandlerKEY(Sess.Obj, Handler);
    },


    .DelHandlerREL = [](input::sess Sess, input::handlerREL Handler)
    {
      ((input::sDriver*)Sess.Drv)->DelHandlerREL(Sess.Obj, Handler);
    },
    
    .DelHandlerWHL = [](input::sess Sess, input::handlerWHL Handler)
    {
      ((input::sDriver*)Sess.Drv)->DelHandlerWHL(Sess.Obj, Handler);
    },

    .DelHandlerABS = [](input::sess Sess, input::handlerABS Handler)
    {
      ((input::sDriver*)Sess.Drv)->DelHandlerABS(Sess.Obj, Handler);
    },

    .DelHandlerKEY = [](input::sess Sess, input::handlerKEY Handler)
    {
      ((input::sDriver*)Sess.Drv)->DelHandlerKEY(Sess.Obj, Handler);
    },

  },
  
};



// Nuc Control
extern "C" kernel::nucStd NucStd
{
  .Ver = kernel::NucVer,

  .Push = []()
  {
    kernel::NucReg(input::Domain, &HAL);
  },
  
};
