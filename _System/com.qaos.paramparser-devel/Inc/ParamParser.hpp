#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>

#include "Basis.hpp"


using namespace std;
using namespace jix;



namespace ParamParser
{
  struct sParams
  {
    vector<string> Modes;
    unordered_map<string, string> Vars;
  };


  extern sParams Params;

  void Parse(iPtr ArgC, char* ArgV[]);
}
