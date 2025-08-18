(*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*)

unit Basis;


interface
uses
  BaseUnix, ctypes;


  // Int
  type  i8   = ctypes.cint8;
  type  i16  = ctypes.cint16;
  type  i32  = ctypes.cint32;
  type  i64  = ctypes.cuint64;
  type  i128 = array[0..1] of i64; // not supported by fpc

  // Int_p
  type  i8_p   = ^i8;
  type  i16_p  = ^i16;
  type  i32_p  = ^i32;
  type  i64_p  = ^i64;
  type  i128_p = ^i128;



  // UInt
  type  u8   = ctypes.cuint8;
  type  u16  = ctypes.cuint16;
  type  u32  = ctypes.cuint32;
  type  u64  = ctypes.cuint64;
  type  u128 = array [0..1] of u64; // not supported by fpc

  // UInt_p
  type  u8_p   = ^u8;
  type  u16_p  = ^u16;
  type  u32_p  = ^u32;
  type  u64_p  = ^u64;
  type  u128_p = ^u128;



  // System
  type  iPtr = IntPtr;
  type  uPtr = UIntPtr;

  type  handle = uPtr;
  type  ohid = uPtr;

  // System_p
  type  iPtr_p = ^iPtr;
  type  uPtr_p = ^uPtr;

  type  handle_p = ^handle;
  type  ohid_p = ^ohid;
  


  // Char
  type  c8 = ctypes.cchar;



  // Bool
  type  bool = Boolean;

  // Bool_p
  type  bool_p = ^bool;


  // IDs
  type  idU = uid_t;
  type  idG = gid_t;
  type  idP = pid_t;
  type  idT = uPtr;


  type  idA = record
  
    Pid: idP;
    Uid: idU;
    Gid: idG;
  end;



  // Float
  type  f32  = ctypes.cfloat;
  type  f64  = ctypes.cdouble;
  type  f128 = array[0..1] of f64; // not supported by fpc

  // Float_p
  type  f32_p  = ^f32;
  type  f64_p  = ^f64;
  type  f128_p = ^f128;



  // Point
  type  point = pointer;
  type  point_p = ^point;
  //#define Nil  NULL

  type  data_ = record
    Point: point;
    Size: uPtr;
  end;
  type data__p = ^data_;


implementation
end.