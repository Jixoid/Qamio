#pragma once

#include <iostream>

#include "JBasic.hpp"

using namespace std;


namespace jix {
  
  enum err: int8u {
    Ok    = 0,  // Everthing is OK
    Unkwn = 1,  // Unknown
    Null  = 2,  // Null pointer
    Mem   = 3,  // Out of Memory
    NF    = 4,  // Not found
    New   = 5,  // Failed New Obj
    Dis   = 6,  // Failed Dispose Obj
    Moun  = 7,  // Not Mounted
    UMoun = 8,  // Not Unmounted
  };

  const string __errs[] = {
    "Everything is OK",   // err::Ok
    "Unknown",            // err::Unkwn
    "Null pointer",       // err::Null
    "Out of Memory",      // err::Mem
    "Object not found",   // err::NF
    "Failed New Obj",     // err::New
    "Failed Dispose Obj", // err::Dis
    "Not Mounted",        // err::Moun
    "Not Unmounted",      // err::UMoun
  };

  const string ErrToStr(err Err) {
    return __errs[Err];
  }
  
}
