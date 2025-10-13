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
#include "Kernel.hh"


using namespace std;
using namespace jix;



namespace hal::audio
{
  inline const char* Domain = "hal.audio";
  

  struct sess
  {
    point Drv;
    point Obj;
  };

  struct meta
  {
    bool Ready;
    bool f;
  };

  enum bufstate: u8
  {
    sFree    = 0,
    sInvalid = 1,
    sReady   = 2,
    sReading = 3,
  };


  enum depth: u8
  {
    format_16i = 1,
    format_24i = 2,
    format_32i = 3,
    format_32f = 4,
  };

  struct audioMode
  {
    depth Depth;
    u32 Rate;
    u8 Channels;
  };


  template <typename __S>
  struct gsDriver
  {
    // General
    void  (*Reset)();
    u32   (*Count)();

      // Start/Stop
      __S  (*Start)(u32 Index);
      void (*Stop) (__S);

      bool (*IsValid)(__S);

      // Modes
      u16       (*ModeC)(__S);
      audioMode (*ModeG)(__S, u16 Mode);

      // Connect
      bool  (*Connect)       (__S, u16 Mode, u32 Frame, u8 BufCount);
      void  (*DisConnect)    (__S);
      bool  (*IsConnected)   (__S);
      bool  (*IsDevConnected)(__S);

      // Buffer
      u32   (*BufferSize)(__S);
      u8    (*BufferC)(__S);
      point (*BufferG)(__S, u8 Index);

      bufstate (*BufferStateGet)(__S, u8 Index);
      void     (*BufferStateSet)(__S, u8 Index, bufstate State);

      u8    (*ActiveBuffer)(__S);
      void  (*JumpBuffer)(__S, u8 Index);
  };
  
  using sDriver = gsDriver<point>;

  struct sHAL
  {
    bool  (*RegDriver)(sDriver *Driver);
    bool  (*DelDriver)(sDriver *Driver);

    gsDriver<audio::sess> DRV;
  };

}
