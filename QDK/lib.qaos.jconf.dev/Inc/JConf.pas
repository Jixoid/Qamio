(*
  This file is part of JConf
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with JConf. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*)



unit JConf;


interface
uses
  Basis;
  


// Types
type  jc_obj = point;

type  jc_val = point;
type  jc_stc = point;
type  jc_arr = point;


// Is
function  jc_IsVal(obj: jc_obj): bool; cdecl; external;
function  jc_IsStc(obj: jc_obj): bool; cdecl; external;
function  jc_IsArr(obj: jc_obj): bool; cdecl; external;


// New / Dis
function  jc_NewVal(): jc_val; cdecl; external;
procedure jc_DisVal(obj: jc_val); cdecl; external;

function  jc_NewStc(): jc_stc; cdecl; external;
procedure jc_DisStc(obj: jc_stc); cdecl; external;

function  jc_NewArr(): jc_arr; cdecl; external;
procedure jc_DisArr(obj: jc_arr); cdecl; external;


// Set/Get
function  jc_ValGet(obj: jc_val): pchar; cdecl; external;
function  jc_ValSet(obj: jc_val; const value: pchar): bool; cdecl; external;
procedure jc_DisStr(str: pchar); cdecl; external;


function jc_StcGet(obj: jc_stc; const scope: pchar): jc_obj; cdecl; external;
function jc_StcSet(obj: jc_stc; const scope: pchar; obje: jc_obj): bool; cdecl; external;
function jc_StcDel(obj: jc_stc; const scope: pchar): bool; cdecl; external;
function jc_StcC  (obj: jc_stc): u32; cdecl; external;
function jc_StcInd(obj: jc_stc; index: u32; Name: ppchar): jc_obj; cdecl; external;


function jc_ArrC   (obj: jc_arr): u32; cdecl; external;
function jc_ArrGet (obj: jc_arr; index: u32): jc_obj; cdecl; external;
function jc_ArrSet (obj: jc_arr; index: u32; obje: jc_obj): bool; cdecl; external;
function jc_ArrPush(obj: jc_arr; obje: jc_obj): bool; cdecl; external;
function jc_ArrDel (obj: jc_arr; index: u32): bool; cdecl; external;



// Parse
function  jc_Parse(fpath: pchar): jc_stc; cdecl; external;
function  jc_ParseRaw(fpath: pchar): jc_stc; cdecl; external;
function  jc_ParseBin(fpath: pchar): jc_stc; cdecl; external;

function  jc_WriteRaw(fpath: pchar; data: jc_stc): bool; cdecl; external;
function  jc_WriteBin(fpath: pchar; data: jc_stc): bool; cdecl; external;


implementation
end.