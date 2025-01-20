#pragma once

#include <stdio.h>
#include <stdint.h>



// Global
typedef uint8_t  err;


// Status  0..255
#ifdef External
  extern const err _err_ok   ;  // Everthing is OK
  extern const err _err_unkwn;  // Unknown
  extern const err _err_null ;  // Null pointer
  extern const err _err_oomem;  // Out of Memory
  extern const err _err_nfobj;  // Object not found

  extern const char* ErrToStr(err Err);
#else
  const err _err_ok    = 0;  // Everthing is OK
  const err _err_unkwn = 1;  // Unknown
  const err _err_null  = 2;  // Null pointer
  const err _err_oomem = 3;  // Out of Memory
  const err _err_nfobj = 4;  // Object not found


  const char* __errs[] = {
    "Everything is OK",  // _err_ok
    "Unknown",           // _err_unkwn
    "Null pointer",      // _err_null
    "Out of Memory",     // _err_oomem
    "Object not found"   // _err_nfobj
  };

  const char* ErrToStr(err Err) {
    return __errs[Err];
  }
#endif

