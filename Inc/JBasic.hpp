#pragma once

#include <iostream>

using namespace std;


namespace jix {

  typedef __int8_t     int8;
  typedef __uint8_t    int8u;
  typedef __int16_t    int16;
  typedef __uint16_t   int16u;
  typedef __int32_t    int32;
  typedef __uint32_t   int32u;
  typedef __int64_t    int64;
  typedef __uint64_t   int64u;
  typedef __int128_t   int128;
  typedef __uint128_t  int128u;


  typedef int32u   uid;
  typedef int32u   gid;

  typedef size_t   ohid;
  typedef char*    point;
  typedef size_t   size;

  typedef int8u   byte;


  struct data {
    size  Size;
    point Point;
  };

}
