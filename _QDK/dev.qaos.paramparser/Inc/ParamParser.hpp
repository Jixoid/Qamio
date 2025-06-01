#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>

#include "Basis.hpp"


using namespace std;
using namespace jix;



namespace jix::ParamParser
{
  struct param
  {
    char *Name;
    bool  Def;
  };

  struct mod
  {
    char  *Name;
    void (*Method)();
    
    param *Params;
    i32    ParamC;

    mod   *Mods;
    i32    ModC;
  };


  void Parse(int ArgC, char *ArgV[], int ModC, mod *ModV,  int &FileC, char **FileV);

}
