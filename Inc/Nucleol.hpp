#pragma once

#include <iostream>

#include "JBasic.hpp"

using namespace std;


namespace jix {

  const int16u gnucVer = 1;

  enum nuc_mode: int8u {
    NotWork  = 0,
    Working  = 1,
    Kaizen   = 2,
    Sleep    = 3,
    Standby  = 4,
    Update   = 5,
  };

  typedef bool (*nucCheck)(int16u Ver);
  typedef void (*nucLoad)();
  typedef void (*nucStart)();
  typedef void (*nucStop)();
  typedef void (*nucUnload)();
}
