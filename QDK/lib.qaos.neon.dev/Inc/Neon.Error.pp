(*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*)



{$mode objfpc}{$H+}
unit Neon.Error;

interface
uses
  Basis;


  type neon_error = u16;
  type neon_errors = (
    neOK       = 0,
    neUknown   = 1,
    neSocket   = 2,
    neConnect  = 3,
    neSend     = 4,
    neRecv     = 5,
    neMemory   = 6,
    neTimeout  = 7,
    neClosed   = 8,
    neInvParam = 9
  );


  function neon_GetLastError(): neon_error; cdecl; external;

  function neon_ErrorStr(err: neon_error): pchar; cdecl; external;

implementation
end.