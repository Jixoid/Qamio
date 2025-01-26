#pragma once

#include <iostream>

#include "JBasic.hpp"
#include "Nucleol.hpp"

using namespace std;

#define ComGet(X,Y) X = (Y)Com.Get(#X);




namespace jix {

  // Global
  typedef point  secon;

  enum eSecPerm: int8u {
    Read     = 1,
    Write    = 2,
    Execute  = 4,
    Delete   = 8,
    See      = 16,
    AuthS    = 32,
  };
  

  // Secons Control
  typedef secon (*secon_New)(uid Owner);
  typedef bool  (*secon_Dis)(secon Self);
  typedef bool  (*secon_Let)(secon Self, uid Client, eSecPerm Perm, bool Append);
  typedef bool  (*secon_Acc)(secon Self, uid Client, eSecPerm Perm);


  namespace Wedling {
    
    secon_New  Secon_New;
    secon_Dis  Secon_Dis;
    secon_Let  Secon_Let;
    secon_Acc  Secon_Acc;

    void PopNuc(sNucCom Com) {
      ComGet(Secon_New,secon_New);
      ComGet(Secon_Dis,secon_Dis);
      ComGet(Secon_Let,secon_Let);
      ComGet(Secon_Acc,secon_Acc);
    }
  }


}
