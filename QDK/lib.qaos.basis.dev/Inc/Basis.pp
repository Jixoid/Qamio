(*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*)



{$Mode objfpc}{$H+}
unit Basis;


interface
uses
  ctypes,
  unixtype,
  cthreads;


  // Int
  type i8  = cint8_t;
  type i16 = cint16_t;
  type i32 = cint32_t;
  type i64 = cint64_t;



  // UInt
  type u8  = cuint8_t;
  type u16 = cuint16_t;
  type u32 = cuint32_t;
  type u64 = cuint64_t;


  // System
  {$if SizeOf(Pointer) = 8}
    type u0 = u64;
    type i0 = i64;
  {$else}
    type u0 = u32;
    type i0 = i32;
  {$endif}

  type handle = u0;
  type ohid   = u0;


  // Float
  type f32 = single;
  type f64 = double;

  {$if SizeOf(Pointer) = 8}
    type f0 = f64;
  {$else}
    type f0 = f32;
  {$endif}



  // IDs
  type idU = uid_t;
  type idG = gid_t;
  type idP = pid_t;
  type idT = pthread_t;


  type idA = record
  
    Pid: idP;
    Uid: idU;
    Gid: idG;
  end;



  // Point
  type point = pointer;

  type data_ = record
  
    Point: point;
    Size: u0;
  end;


implementation
end.
