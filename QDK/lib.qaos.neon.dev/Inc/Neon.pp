(*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*)



{$mode objfpc}{$H+}
unit Neon;


interface
uses
  Basis, Neon.Error;



  // Types
  type neon_contactType = u8;
  type neon_contactTypes = (
		Host = 1,
    P2P  = 2
  );

  type neon_contact = record
  
    Type_: neon_contactType;
    Aid: idA;
  end;

  type contact_p = ^neon_contact;


  type neon_mAccept  = function(Aid: idA): bool; cdecl;
	type neon_mHandler = procedure(Con: contact_p; Data: data_); cdecl;



  // Methods
  function neon_Connect(ID: pchar): contact_p; cdecl; external;
  function neon_Server (ID: pchar; ConCount: u32): contact_p; cdecl; external;
  function neon_Close  (Con: contact_p): bool; cdecl; external;

  function neon_Send(Con: contact_p; Data: data_): bool; cdecl; external;
  function neon_Recv(Con: contact_p; Data: data__p): bool; cdecl; external;

  function neon_Process(Con: contact_p; Work: bool_p; Accept: neon_mAccept; Handler: neon_mHandler): bool; cdecl; external;


implementation
end.