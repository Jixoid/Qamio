#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>
#include <string.h>

#include "JBasic.h"
#include "Nucleol.h"
#include "Wedling.h"

#define toSec(ptr) ((__secon*)(ptr))


typedef struct {
  ohid       Client;
  secon_perm Perm;
} __secon_auth;

typedef struct {
  ohid          Owner;
  secon_perm    Other;
  uint16_t      AuthC;
  __secon_auth* Auth;
} __secon;


#ifndef Wedling

secon secon_New(ohid nOwner) {
  return 1;
}

bool  secon_Dis(secon nSelf) {
  return true;
}

bool  secon_Let(secon nSelf, ohid nClient, secon_perm nPerm, bool Append) {
  return true;
}

bool  secon_Acc(secon nSelf, ohid nClient, secon_perm nPerm) {
  return true;
}

#else

secon secon_New(ohid nOwner) {
  __secon* result = malloc( sizeof(__secon) );

  result->Owner = nOwner;
  result->Other = 0;
  result->AuthC = 0;
  result->Auth  = NULL;

  return result;
}

bool  secon_Dis(secon nSelf) {
  if (nSelf == NULL)
    return false;

  if (toSec(nSelf)->Auth != NULL) {
    free(toSec(nSelf)->Auth);
  }

  free(nSelf);

  return true;
}

bool  secon_Let(secon nSelf, ohid nClient, secon_perm nPerm, bool Append) {
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
  for (uint16_t i = 0; i < toSec(nSelf)->AuthC; i++) {
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
    toSec(nSelf)->Auth = realloc(
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

bool  secon_Acc(secon nSelf, ohid nClient, secon_perm nPerm) {
  if (nSelf == NULL)
    return false;

  // Owner
  if (toSec(nSelf)->Owner == nClient)
    return true;

  // Other
  if (nClient == 0)
    return ((toSec(nSelf)->Other & nPerm) != 0);

  // Members
  for (uint16_t i = 0; i < toSec(nSelf)->AuthC; i++) {
    if (toSec(nSelf)->Auth[i].Client != nClient)
      continue;

    return ((toSec(nSelf)->Auth[i].Perm & nPerm) != 0);
  }

  return false;
}

data  secon_Get(secon nSelf) {
  data result;

  if (nSelf == NULL) {
    result.Point = NULL;
    return result;
  }

  // Calc size
  result.Size = sizeof(__secon) -sizeof(__secon_auth*) 
    +(sizeof(__secon_auth)*(toSec(nSelf)->AuthC));

  // Malloc
  result.Point = malloc(result.Size);
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

bool  secon_Set(secon nSelf, data nData) {
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
    toSec(nSelf)->Auth = realloc(
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

#endif
