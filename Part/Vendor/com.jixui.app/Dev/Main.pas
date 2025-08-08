(*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*)



uses
  Basis, JConf, Neon, FWK.Argon;

var
  NSysUI: contact_p;



function New_Window(W,H: u32; F: u32 = 0): u64;
var
  CMD: FWK.Argon.cmd_New;
  Data: data_;
begin
  CMD := FWK.Argon.cmd_New.New();

  CMD.W := W;
  CMD.H := H;
  CMD.Flags := F;

  Data.Point := @CMD;
  Data.Size  := sizeof(CMD);

  neon_Send(NSysUI, Data);
end;



begin
  NSysUI := neon_Connect('com.qaos.sysui');
  if (NSysUI = Nil) then
  begin
    Writeln('Not Connected');
    halt(1);
  end;

  New_Window(400,300);


  Writeln('Hello');
end.
