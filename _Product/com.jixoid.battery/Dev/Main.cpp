#include <iostream>
#include <vector>
#include <atomic>
#include <thread>
#include <filesystem>
namespace fs = std::filesystem;

#include "Basis.hpp"
#include "Nucleol.hpp"

using namespace std;
using namespace jix;


#define ef  else if

#define PopNuc(X)  X = *(s##X*)NucCom.Get(#X);
#define PushNuc(X) NucCom = Com; NucCom.Set(#X, &X); X =

#define Log(X)     logger(__func__, X)
#define Log2(X,Y)  logger(__func__, X, Y)



vector<pair<string, battery_Info>> Bats;

atomic<bool> Active;



inline battery_Info BatteryRead(string Path)
{
  battery_Info Ret;


  _l_Status: {
    ifstream File(Path +"status");
  
    if (!File.is_open())
      Log2("File not opened: "+ Path+"status", lFatal);


    // Read
    string Cac;
    getline(File, Cac);


    if (Cac == "Discharging")
      Ret.Status = battery_Status::bsDisCharging;

    ef (Cac == "Charging")
      Ret.Status = battery_Status::bsCharging;

    ef (Cac == "Full")
      Ret.Status = battery_Status::bsFull;

    ef (Cac == "Not charging")
      Ret.Status = battery_Status::bsNotCharging;

    else
      Ret.Status = battery_Status::bsUnknown;
  }

  _l_Percent: {
    ifstream File(Path +"capacity");
  
    if (!File.is_open())
      Log2("File not opened: "+ Path+"capacity", lFatal);


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
  for (auto &X: Bats)
    X.second = BatteryRead(X.first);
  

  this_thread::sleep_for(3s);
}}



#pragma region Publish

void _Reset()
{
  auto mIsBattery = [](string Path)-> bool
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
  


  Bats.clear();

  for (auto &Entry: fs::directory_iterator("/sys/class/power_supply"))
    if (fs::is_directory(Entry.path()) && mIsBattery(Entry.path()))
      Bats.push_back({
        Entry.path().filename().string(),
        BatteryRead(Entry.path().filename().string()),
      });
  
}


u16 _Count()
{
  return Bats.size();
}

battery_Info _Get(u16 Index)
{
  return Bats[Index].second;
}

#pragma endregion






// Global
sNucCom NucCom;


#pragma region NucCom

void Push(sNucCom Com)
{
  PushNuc(Battery)
  {
    .Reset = &_Reset,

    .Count = &_Count,
    .Get   = &_Get,
  };
}


void Load()
{
  _Reset();
}


extern "C" sNucStd NucStd
{
  .Ver = NucVer,

  .Push = &Push,

  .Load = &Load,
};

#pragma endregion
