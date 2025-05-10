#pragma once

#include <iostream>

#include <stdint.h>
#include <stdatomic.h>

using namespace std;



namespace jix
{  

  // Int
  using i8   = int8_t;
  using i16  = int16_t;
  using i32  = int32_t;
  using i64  = int64_t;



  // UInt
  using u8   = uint8_t;
  using u16  = uint16_t;
  using u32  = uint32_t;
  using u64  = uint64_t;



  // System
  using iPtr = intptr_t;
  using uPtr = uintptr_t;
  
  using handle = uPtr;
  using ohid   = uPtr;
  


  // Char
  using c8  = char;
  using c16 = char16_t;
  using c32 = char32_t;



  // Float
  using f32 = float;
  using f64 = double;

  

  // IDs
  using idU = uid_t;
  using idG = gid_t;
  using idP = pid_t;
  using idT = pthread_t;
  

  // Point
  using point = void*;
  #define Nil  nullptr

  struct sData
  {
    point Point;
    uPtr  Size;
  };


  // String
  struct jstring
  {
    void (*Dis)(jstring*);
    char* Str;
  };



  // Atomic
  template<typename _T>
  struct atom
  {
    _Atomic _T  Obj;


    void operator++()
    {
      atomic_fetch_add(&Obj, 1);
    }

    void operator--()
    {
      atomic_fetch_sub(&Obj, 1);
    }


    void Push(_T Val)
    {
      atomic_store(&Obj, Val);
    }

    _T Pop()
    {
      return atomic_load(&Obj);
    }
  
  };

}
