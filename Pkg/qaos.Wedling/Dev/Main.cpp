#include <iostream>
#include <unordered_map>

#include "JBasic.hpp"
#include "Nucleol.hpp"
#include "Wedling.hpp"

#define ComSet(X) NucCom.Set(#X, (point)X);


using namespace std;
using namespace jix;


struct __secon {
  uid   Owner;
  int8u  Other;
  unordered_map<ohid, int8u> Auths;
};


__secon* Secon_New(uid Owner) {

  __secon* Res = new __secon;

  Res->Owner = Owner;
  Res->Other = 0;

  return Res;
}

bool  Secon_Dis(__secon* Self) {

  if (Self == NULL)
    return false;


  delete Self;

  return true;
}

bool  Secon_Let(__secon* Self, uid Client, int8u Perm, bool Append) {

  if (Self == NULL)
    return false;

  // Owner
  if (Self->Owner == Client)
    return Append;

  // Other
  if (Client == 0) {
    if (Append)
      Self->Other |= Perm;
    else
      Self->Other &= ~Perm;

    return true;
  }

  // Members
  int8u Def;

  auto it = Self->Auths.find(Client);
  if (it != Self->Auths.end()) 
    Def = it->second;
  else
    Def = Self->Other;


  if (Append)
    Def |= Perm;
  else
    Def &= ~Perm;

  
  Self->Auths[Client] = Def;

  return true;
}

bool  Secon_Acc(__secon* Self, uid Client, int8u Perm) {

  if (Self == NULL)
    return false;

  // Root
  if (Client == 0)
    return true;

  // Owner
  if (Self->Owner == Client)
    return true;
  
  // Find
  auto it = Self->Auths.find(Client);
  if (it != Self->Auths.end()) {
    // Members
    return ((it->second & Perm) != 0);

  } else {
    // Other
    return ((Self->Other & Perm) != 0);    
  }

}



// Global
sNucCom NucCom;


#pragma region NucCom
extern "C" bool NucCheck(int16u Ver) {
  return (gnucVer == Ver);
}


extern "C" void NucPush(sNucCom Com) {

  NucCom = Com;

  ComSet(Secon_New);
  ComSet(Secon_Dis);
  ComSet(Secon_Let);
  ComSet(Secon_Acc);
}

extern "C" void NucPop() {

}


extern "C" void NucLoad() {
  
}

extern "C" void NucUnload() {
  
}


extern "C" void NucStart() {
  
}

extern "C" void NucStop() {
  
}

#pragma endregion
