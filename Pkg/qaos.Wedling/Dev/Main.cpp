#include <iostream>
#include <unordered_map>

#include "JBasic.hpp"
#include "Nucleol.hpp"
#include "Wedling.hpp"

#define toSec(X) ((__secon*)(X))
#define ComSet(X) NucCom.Set(#X, (point)X);


using namespace std;
using namespace jix;


struct __secon {
  uid   Owner;
  int8u  Other;
  unordered_map<ohid, int8u> Auths;
};


secon Secon_New(uid nOwner) {

  __secon* result = new __secon;

  result->Owner = nOwner;
  result->Other = 0;

  return (point)result;
}

bool  Secon_Dis(secon nSelf) {

  if (nSelf == NULL)
    return false;


  delete toSec(nSelf);

  return true;
}

bool  Secon_Let(secon nSelf, uid nClient, int8u nPerm, bool Append) {

  if (nSelf == NULL)
    return false;

  // Owner
  if (toSec(nSelf)->Owner == nClient)
    return Append;

  // Other
  if (nClient == 0) {
    if (Append)
      toSec(nSelf)->Other |= nPerm;
    else
      toSec(nSelf)->Other &= ~nPerm;

    return true;
  }

  // Members
  int8u Def;

  auto it = toSec(nSelf)->Auths.find(nClient);
  if (it != toSec(nSelf)->Auths.end()) 
    Def = it->second;
  else
    Def = toSec(nSelf)->Other;


  if (Append)
    Def |= nPerm;
  else
    Def &= ~nPerm;

  
  toSec(nSelf)->Auths[nClient] = Def;

  return true;
}

bool  Secon_Acc(secon nSelf, uid nClient, int8u nPerm) {

  if (nSelf == NULL)
    return false;

  // Owner
  if (toSec(nSelf)->Owner == nClient)
    return true;
  
  // Find
  auto it = toSec(nSelf)->Auths.find(nClient);
  if (it != toSec(nSelf)->Auths.end()) {
    // Members
    return ((it->second & nPerm) != 0);

  } else {
    // Other
    return ((toSec(nSelf)->Other & nPerm) != 0);    
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
