#pragma once

#include <iostream>

#include "JBasic.hpp"

using namespace std;


namespace jix {

  const int16u gnucVer = 1;

  enum eNucMode: int8u {
    NotWork  = 0,
    Working  = 1,
    Kaizen   = 2,
    Sleep    = 3,
    Standby  = 4,
    Update   = 5,
  };

  struct sNucCom {
    typedef void  (*set)(string, point);
    typedef point (*get)(string);

    set Set;
    get Get;
  };


  typedef bool (*nucCheck)(int16u Ver);
  
  typedef void (*nucPush)(sNucCom Com);
  typedef void (*nucPop)();
  
  typedef void (*nucLoad)();
  typedef void (*nucUnload)();

  typedef void (*nucStart)();
  typedef void (*nucStop)();

  typedef bool     (*nucSetStatus)(eNucMode Mode);
  typedef eNucMode (*nucGetStatus)();
}
