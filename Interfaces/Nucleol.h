#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>



// Global
typedef uint8_t  nuc_mode;


// Status  0..255
#ifdef External
  extern const nuc_mode  _nuc_NotWork; // Not Working
  extern const nuc_mode  _nuc_Working; // Working
  extern const nuc_mode  _nuc_Kaizen ; // Kaizen
  extern const nuc_mode  _nuc_Sleep  ; // Sleeping
  extern const nuc_mode  _nuc_Standby; // Standby
  extern const nuc_mode  _nuc_Update ; // Updating
#else
  const nuc_mode  _nuc_NotWork = 0; // Not Working
  const nuc_mode  _nuc_Working = 1; // Working
  const nuc_mode  _nuc_Kaizen  = 2; // Kaizen
  const nuc_mode  _nuc_Sleep   = 3; // Sleeping
  const nuc_mode  _nuc_Standby = 4; // Standby
  const nuc_mode  _nuc_Update  = 5; // Updating
#endif


// Nucleol Protocol
typedef struct {
  uint16_t Main;
  uint16_t Sub;
  uint16_t Patch;
} ver;


bool NucInfo(ver Ver);
