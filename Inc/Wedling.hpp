#pragma once

#include <iostream>

#include "JBasic.hpp"
#include "Nucleol.hpp"

using namespace std;


namespace jix {

  // Global
  typedef point  secon;

  enum secon_perm: int8u {
    R  = 1,   // Read
    W  = 2,   // Write
    X  = 4,   // Execute
    D  = 8,   // Delete
    S  = 16,  // See
    A  = 32,  // Auth Share
  };


  // Wedling Control
  nuc_mode Wedling_getMode();
  bool     Wedling_getEnforcing();
  bool     Wedling_setEnforcing(bool Enforcing);


  // Secons Control
  typedef secon (*secon_New)(ohid Owner);
  typedef bool  (*secon_Dis)(secon Self);
  typedef bool  (*secon_Let)(secon Self, ohid Client, secon_perm Perm, bool Append);
  typedef bool  (*secon_Acc)(secon Self, ohid Client, secon_perm Perm);
  typedef data  (*secon_Get)(secon Self);
  typedef bool  (*secon_Set)(secon Self, data Data);

}
