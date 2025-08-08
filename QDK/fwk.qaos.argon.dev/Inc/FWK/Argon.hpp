/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#pragma once

#include <iostream>

#include "Basis.h"


using namespace std;
using namespace jix;



namespace fwk::argon
{
  inline const char* Domain = "fwk.argon";
  


  enum commands: u32
  {
    acErr = 0x01,
    acNew = 0x02,
    acDis = 0x03,
  };



  struct cmd_Err_r
  {
    u32 OpCode; // commands::acErr;
  };

  struct cmd_New
  {
    u32 OpCode = commands::acNew;
    u32 W,H;
    u32 Flags = 0;
    u64 Parent = 0;
    u64 Time;
  };

  struct cmd_New_r
  {
    u32 OpCode; // commands::acNew;
    u32 __pad;

    u64 OHID;
    u64 MsgTime;
  };


  struct cmd_Dis
  {
    u32 OpCode = commands::acDis;
    u32 __pad;
    u64 OHID;
  };
  
}
