#pragma once

#include <iostream>

using namespace std;



#define Nil nullptr

#define int8     __int8_t
#define int8u    __uint8_t
#define int16    __int16_t
#define int16u   __uint16_t
#define int32    __int32_t
#define int32u   __uint32_t
#define int64    __int64_t
#define int64u   __uint64_t
#define int128   __int128_t
#define int128u  __uint128_t

#define int32f   float
#define int64f   double
#define int128f  __float128


namespace jix {

  typedef uid_t   uid;
  typedef gid_t   gid;

  typedef size_t   ohid;
  typedef char*    point;
  typedef size_t   size;


  struct data {
    size  Size;
    point Point;
  };

}
