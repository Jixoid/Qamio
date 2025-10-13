/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#pragma once

#include <iostream>
#include <string.h>
#include <memory.h>

#include "Basis.h"


using namespace std;
using namespace jix;



namespace fwk::argon
{
  inline const char* Domain = "fwk.argon";
  


  enum commands: u32
  {
    acErr       = 0x01,
    acNew       = 0x02,
    acDis       = 0x03,
    acShow      = 0x04,
    acResize    = 0x05,
    acSetTitle  = 0x06,
    acSetStatus = 0x07,
    acSetProg   = 0x08,
  };


  struct WIN_Err_r
  {
    u32 OpCode = commands::acErr;
    u32 __pad;
    ohid OHID;

    u64 MsgTime;
  };


  struct WIN_New
  {
    u32 OpCode = commands::acNew;
    u32 W,H;
    u32 Flags = 0;
    u64 Parent = 0;
    u64 Time; // TimeStamp
  };

  struct WIN_New_r
  {
    u32 OpCode = commands::acNew; // commands::acNew;
    u32 __pad;
    
    ohid OHID;
    u64  MsgTime; // MsgTime
  };

  struct WIN_Dis
  {
    u32 OpCode = commands::acDis;
    u32 __pad;
    ohid OHID;
  };


  enum showState: u32
  {
    ssNormal = 1,
    ssMini = 2,
    ssMaxi = 3,
    ssFull = 4,
    ssDock = 5,
  };

  struct WIN_Show_r
  {
    u32 OpCode = commands::acShow;
    showState State;
    ohid OHID;
  };


  struct WIN_Resize_r
  {
    u32 OpCode = commands::acResize;
    u32 __pad;
    ohid OHID;
    u32 SizeW,SizeH;
  };


  struct WIN_SetTitle
  {
    u32 OpCode = commands::acSetTitle;
    u32 Size;
    ohid OHID;
    char *Msg = Nil;


    inline data_ Data(char *Msg)
    {
      data_ Ret;

      this->Size = strlen(Msg)+1;

      Ret.Size = sizeof(WIN_SetTitle) +this->Size;
      Ret.Point = malloc(Ret.Size);

      memcpy(Ret.Point, this, sizeof(WIN_SetTitle));
      memcpy((char*)Ret.Point +sizeof(WIN_SetTitle), Msg, this->Size);

      return Ret;
    }
  };


  enum winState: u32
  {
    wsNormal = 1,
    wsProgress = 2,
    wsAlert = 3,
    wsError = 4,
  };

  struct WIN_SetStatus
  {
    u32 OpCode = commands::acSetStatus;
    winState State;
    ohid OHID;
    bool Progress;
  };


  struct WIN_SetProgress
  {
    u32 OpCode = commands::acSetProg;
    ohid OHID;
    u32 Max, Value;
  };

}
