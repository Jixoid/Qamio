/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#define ef else if
#define el else

#include <iostream>
#include <cmath>
#include <format>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <filesystem>

#include "Basis.h"
#include "Kernel.hh"

#include "HAL/Battery.hh"

#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>


using namespace std;
using namespace jix;
using namespace hal;

using namespace qaos;

namespace fs = std::filesystem;


#define Log(X)     kernel::Logger(__func__, X)
#define Log2(X,Y)  kernel::Logger(__func__, X, Y)



atomic<bool> Active;
thread *Thread = Nil;
shared_mutex Mutex;

struct __battSess
{
  string Path;
  bool Active = false;
  battery::battery_Info Info = {0, battery::bsUnknown};
};

vector<__battSess> Bats;




bool isBattery(string Path)
{
  ifstream File(Path +"type");
  string Cac;
  
  if (File.is_open())
  {
    getline(File, Cac);
    return (Cac == "Battery");
  }

  return false;
};

battery::battery_Info BatteryRead(string Path)
{
  battery::battery_Info Ret;


  _l_Status: {
    ifstream File(Path +"status");
  
    if (!File.is_open())
      Log2("File not opened: "+ Path+"status", kernel::lPanic);


    // Read
    string Cac;
    getline(File, Cac);


    if (Cac == "Discharging")
      Ret.Status = battery::battery_Status::bsDisCharging;

    ef (Cac == "Charging")
      Ret.Status = battery::battery_Status::bsCharging;

    ef (Cac == "Full")
      Ret.Status = battery::battery_Status::bsFull;

    ef (Cac == "Not charging")
      Ret.Status = battery::battery_Status::bsNotCharging;

    else
      Ret.Status = battery::battery_Status::bsUnknown;
  }

  _l_Percent: {
    ifstream File(Path +"capacity");
  
    if (!File.is_open())
      Log2("File not opened: "+ Path+"capacity", kernel::lPanic);


    // Read
    string Cac;
    getline(File, Cac);

    Ret.Percent = atoi(Cac.c_str());
  }


  // Return
  return Ret;
}


void Handler() { while (Active.load())
{
  unique_lock Lock(Mutex);

  for (auto &X: Bats)
    if (X.Active)
      X.Info = BatteryRead(X.Path);
  
  this_thread::sleep_for(3s);
}}



// Global
battery::sDriver DRV = {

  #define Self ((__battSess*)__Self)

  // General
  .Reset = []()
  {
    unique_lock Lock(Mutex);

    Bats.clear();

    for (auto &Entry: fs::directory_iterator("/sys/class/power_supply"))
      if (fs::is_directory(Entry.path()) && isBattery(Entry.path()))
        Bats.push_back((__battSess){
          .Path = Entry.path().filename().string()
        });
  },

  .Count = []() -> u32
  {
    return Bats.size();
  },


  .Start = [](u32 Index) -> point
  {
    Bats[Index].Active = true;

    return &Bats[Index];
  },

  .Stop = [](point __Self)
  {
    Self->Active = false;
  },


  .IsValid = [](point __Self) -> bool
  {
    return Self->Active;
  },

  .Get = [](point __Self) -> battery::battery_Info
  {
    return Self->Info;
  },

  #undef Self
};

battery::sinfo INF = {
  .Name = "JixUI generic battery driver",
};



// Nuc Control
battery::sHAL *HAL = Nil;



extern "C" kernel::nucStd NucStd
{
  .Ver = kernel::NucVer,

  .Load = []()
  {
    Active.store(true);
    Thread = new thread(&Handler);

    DRV.Reset();
  },

  .Unload = []()
  {
    if (HAL != Nil)
      HAL->DelDriver(&DRV);

    
    Active.store(false);
    if (Thread->joinable())
      Thread->join();
  },


  .Pop = []()
  {
    HAL = (decltype(HAL))kernel::NucGet(battery::Domain);
    if (HAL == Nil)
    {
      Log2(string(battery::Domain) +" is not found", kernel::lWarn);
      Log2("Module self-detach", kernel::lError);

      // Cancel installation
      kernel::NucDetach(Module);

      return;
    }
    
  },

  .Push_Drv = []()
  {
    HAL->RegDriver(&DRV, &INF);
  },

};
