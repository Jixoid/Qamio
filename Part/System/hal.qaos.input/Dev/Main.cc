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
unordered_map<input::sDriver*, input::sinfo*> Drivers;



input::sHAL HAL = {

  .RegDriver = [](input::sDriver *Driver, input::sinfo *Info) -> bool
  {
    // If already exists
    auto X = Drivers.find(Driver);
    
    if (X != Drivers.end())
      return false;

    // Push
    Drivers[Driver] = Info;

    // Log
    #ifdef CONFIG_HAL_input_debug
      Log2(format("RegDrv: {}", Info->Name), kernel::lDebug);
    #endif

    return true;
  },
  
  .DelDriver = [](input::sDriver *Driver) -> bool
  {
    // If not exits
    auto X = Drivers.find(Driver);

    if (X == Drivers.end())
      return false;
    
    // Log
    #ifdef CONFIG_HAL_input_debug
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
        ((hal::input::sDriver*)X.first)->Reset();
    },

    .Count = []() -> u32
    {
      u32 Ret = 0;

      for (auto &X: Drivers)
        Ret += ((hal::input::sDriver*)X.first)->Count();

      return Ret;
    },
  

    .Start = [](u32 Index) -> input::sess
    {
      u32 Offset = 0;

      for (auto &X: Drivers)
      {
        u32 Count = ((input::sDriver*)X.first)->Count();

        if (Index < Offset +Count)
          return
            {
              .Drv = X.first,
              .Obj = ((input::sDriver*)X.first)->Start(Index - Offset)
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
