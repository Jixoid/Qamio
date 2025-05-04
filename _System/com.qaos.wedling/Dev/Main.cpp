#include <iostream>
#include <unordered_map>

#include "Basis.hpp"
#include "Nucleol.hpp"

using namespace std;
using namespace jix;


#define PopNuc(X)  X = *(s##X*)NucCom.Get(#X);
#define PushNuc(X) NucCom = Com; NucCom.Set(#X, &X); X =




struct __secon: qobject
{
  idU Owner;
  u8  Other;
  unordered_map<idU, u8> Auths;
};


#define Self  ((__secon*)__Self)

void  secon_Dis(secon __Self)
{
  if (Self == Nil)
    qexcept(__func__, "Nil pointer");


  delete Self;
}



secon secon·New(idU Owner)
{
  __secon *Ret = new __secon();
  Ret->Dis = &secon_Dis;


  Ret->Owner = Owner;
  Ret->Other = 0000;

  return (secon)Ret;
}

void  secon·Def(secon __Self, u8 Perm)
{
  if (Self == Nil)
    qexcept(__func__, "Nil pointer");


  Self->Other = Perm;
}

bool  secon·Let(secon __Self, idU Client, u8 Perm, bool Append)
{
  if (Self == Nil)
    qexcept(__func__, "Nil pointer");


  // Owner
  if (Self->Owner == Client)
    return Append;


  // Members
  u8 Def;

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

bool  secon·Acc(secon __Self, idU Client, u8 Perm)
{
  if (Self == Nil)
    qexcept(__func__, "Nil pointer");
    

  // Root
  if (Client == 0)
    return true;


  // Owner
  if (Self->Owner == Client)
    return true;
  

  // Find
  auto it = Self->Auths.find(Client);
  if (it != Self->Auths.end())
    return ((it->second & Perm) != 0);   // Member

  else
    return ((Self->Other & Perm) != 0);  // Other

}

#undef Self




// Global
sNucCom NucCom;


#pragma region NucCom

void Push(sNucCom Com)
{
  PushNuc(Wedling)
  {
    .Secon = {
      .New = &secon·New,
      .Def = &secon·Def,
      .Let = &secon·Let,
      .Acc = &secon·Acc,
    }
  };
}


extern "C" sNucStd NucStd
{
  .Ver = NucVer,

  .Push = &Push,
};

#pragma endregion
