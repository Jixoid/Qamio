#include <iostream>

#include "JBasic.hpp"
#include "Nucleol.hpp"
#include "Wedling.hpp"

extern "C" {
  #include "malloc.h"
  #include "string.h"
}

#define toSec(ptr) ((__secon*)(ptr))


using namespace std;
using namespace jix;


typedef struct {
  ohid   Client;
  int8u  Perm;
} __secon_auth;

typedef struct {
  ohid          Owner;
  int8u         Other;
  int16u        AuthC;
  __secon_auth* Auth;
} __secon;


extern "C" secon Secon_New(ohid nOwner) {
  __secon* result = (__secon*)malloc( sizeof(__secon) );

  result->Owner = nOwner;
  result->Other = 0;
  result->AuthC = 0;
  result->Auth  = NULL;

  return (point)result;
}

extern "C" bool  Secon_Dis(secon nSelf) {
  if (nSelf == NULL)
    return false;

  if (toSec(nSelf)->Auth != NULL) {
    free(toSec(nSelf)->Auth);
  }

  free(nSelf);

  return true;
}

extern "C" bool  Secon_Let(secon nSelf, ohid nClient, int8u nPerm, bool Append) {
  if (nSelf == NULL)
    return false;

  // Owner
  if (toSec(nSelf)->Owner == nClient)
    return Append;

  // Others
  if (nClient == 0) {
    if (Append)
      toSec(nSelf)->Other |= nPerm;
    else
      toSec(nSelf)->Other &= ~nPerm;

    return true;
  }

  // Members
  for (int16u i = 0; i < toSec(nSelf)->AuthC; i++) {
    if (toSec(nSelf)->Auth[i].Client != nClient)
      continue;

    if (Append)
      toSec(nSelf)->Auth[i].Perm |= nPerm;
    else
      toSec(nSelf)->Auth[i].Perm &= ~nPerm;
      // Buradaki şişen yapıyı düzelt


    return true;
  }

  // Create a new
  {
    toSec(nSelf)->AuthC += 1;
    toSec(nSelf)->Auth = (__secon_auth*)realloc(
      toSec(nSelf)->Auth, 
      sizeof(__secon_auth)*(toSec(nSelf)->AuthC)
    );
    if (toSec(nSelf)->Auth == NULL)
      return false;

    toSec(nSelf)->Auth[toSec(nSelf)->AuthC-1].Client = nClient;
    toSec(nSelf)->Auth[toSec(nSelf)->AuthC-1].Perm   = nPerm;

    return true;
  }
}

extern "C" bool  Secon_Acc(secon nSelf, ohid nClient, int8u nPerm) {
  if (nSelf == NULL)
    return false;

  // Owner
  if (toSec(nSelf)->Owner == nClient)
    return true;

  // Other
  if (nClient == 0)
    return ((toSec(nSelf)->Other & nPerm) != 0);

  // Members
  for (int16u i = 0; i < toSec(nSelf)->AuthC; i++) {
    if (toSec(nSelf)->Auth[i].Client != nClient)
      continue;

    return ((toSec(nSelf)->Auth[i].Perm & nPerm) != 0);
  }

  return false;
}

extern "C" jix::data  Secon_Get(secon nSelf) {
  jix::data result;

  if (nSelf == NULL) {
    result.Point = NULL;
    return result;
  }

  // Calc size
  result.Size = sizeof(__secon) -sizeof(__secon_auth*) 
    +(sizeof(__secon_auth)*(toSec(nSelf)->AuthC));

  // Malloc
  result.Point = (point)malloc(result.Size);
  if (!result.Point)
    return result;

  // Copy
  memcpy(
    result.Point,
    nSelf,
    sizeof(__secon)
  );

  if (toSec(nSelf)->AuthC != 0)
    memcpy(
      result.Point +sizeof(__secon) -sizeof(__secon_auth*),
      toSec(nSelf)->Auth,
      sizeof(__secon_auth)*(toSec(nSelf)->AuthC)
    );

  return result;
}

extern "C" bool  Secon_Set(secon nSelf, jix::data nData) {
  if (nSelf == NULL || nData.Point == NULL)
    return false;

  // Copy
  memcpy(
    nSelf,
    nData.Point,
    sizeof(__secon) -sizeof(__secon_auth*)
  );

  if (toSec(nSelf)->AuthC == 0)
    toSec(nSelf)->Auth = NULL;
  else {
    toSec(nSelf)->Auth = (__secon_auth*)realloc(
      toSec(nSelf)->Auth, 
      sizeof(__secon_auth)*(toSec(nSelf)->AuthC)
    );
    if (toSec(nSelf)->Auth == NULL)
      return false;

    memcpy(
      toSec(nSelf)->Auth,
      nData.Point +sizeof(__secon) -sizeof(__secon_auth*),
      sizeof(__secon_auth)*(toSec(nSelf)->AuthC)
    );
  }

  return true;
}

