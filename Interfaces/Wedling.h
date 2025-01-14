#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "JBasic.h"
#include "Nucleol.h"

#ifdef External
  #define ex extern
#else
  #define ex
#endif


// Global
typedef point  secon;
typedef byte   secon_perm;


// Auth  1..128
#ifdef External
  extern const secon_perm  _secon_R;  // Read
  extern const secon_perm  _secon_W;  // Write
  extern const secon_perm  _secon_X;  // Execute
  extern const secon_perm  _secon_D;  // Delete
  extern const secon_perm  _secon_S; // See
  extern const secon_perm  _secon_A; // Auth Share
#else
  const secon_perm  _secon_R = 1;  // Read
  const secon_perm  _secon_W = 2;  // Write
  const secon_perm  _secon_X = 4;  // Execute
  const secon_perm  _secon_D = 8;  // Delete
  const secon_perm  _secon_S = 16; // See
  const secon_perm  _secon_A = 32; // Auth Share
#endif


// Wedling Control
nuc_mode Wedling_getMode();
bool     Wedling_getEnforcing();
bool     Wedling_setEnforcing(bool Enforcing);


// Secons Control
secon secon_New(ohid Owner);
bool  secon_Dis(secon Self);
bool  secon_Let(secon Self, ohid Client, secon_perm Perm, bool Append);
bool  secon_Acc(secon Self, ohid Client, secon_perm Perm);
data  secon_Get(secon Self);
bool  secon_Set(secon Self, data Data);
