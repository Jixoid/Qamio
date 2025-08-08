(*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*)



{$ModeSwitch AdvancedRecords}
unit FWK.Argon;

interface
uses
  Basis;



const Domain: string = 'fwk.argon';


type commands = (
  acErr = $01,
  acNew = $02,
  acDis = $03
);



type cmd_Err_r = record
public
  var OpCode: u32; // commands::acErr;
end;

type cmd_New = record
public
  class function New(): cmd_New; static;

public
  var OpCode: u32;
  var W,H: u32;
  var Flags: u32;
  var Parent: u64;
  var Time: u64;
end;

type cmd_New_r = record
public
  var OpCode: u32;  // commands::acNew;
  var __pad: u32;

  var OHID: u64;
  var MsgTime: u64;
end;


type cmd_Dis = record
public
  class function New(): cmd_Dis; static;

public
  var OpCode: u32;
  var __pad: u32;
  var OHID: u64;
end;



implementation


class function cmd_New.New(): cmd_New;
begin
  Result.OpCode := u32(commands.acNew);
  Result.Flags := 0;
  Result.Parent := 0;
end;

class function cmd_Dis.New(): cmd_Dis;
begin
  Result.OpCode := u32(commands.acDis);
end;


end.