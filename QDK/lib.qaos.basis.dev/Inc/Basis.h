/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <bits/types.h>
#include <bits/pthreadtypes.h>


#ifdef __cplusplus
extern "C" {
namespace jix {}
#endif


// Int
typedef  int8_t   i8;
typedef  int16_t  i16;
typedef  int32_t  i32;
typedef  int64_t  i64;



// UInt
typedef  uint8_t   u8;
typedef  uint16_t  u16;
typedef  uint32_t  u32;
typedef  uint64_t  u64;


// Char
typedef  char  c8;


// System
#if __WORDSIZE == 64
  typedef u64 u0;
  typedef i64 i0;
#else
  typedef u32 u0;
  typedef i32 i0;
#endif

typedef  u0  handle;
typedef  u0  ohid;


// Float
typedef  float   f32;
typedef  double  f64;

#if __WORDSIZE == 64
  typedef f64 f0;
#else
  typedef f32 f0;
#endif



// IDs
typedef  __uid_t  idU;
typedef  __gid_t  idG;
typedef  __pid_t  idP;
typedef  pthread_t  idT;


typedef struct
{
  idP Pid;
  idU Uid;
  idG Gid;
} idA;



// Point
typedef  void*  point;
#define Nil  nullptr

typedef struct
{
  point Point;
  u0    Size;
} data_;


#ifdef __cplusplus
}
#endif
