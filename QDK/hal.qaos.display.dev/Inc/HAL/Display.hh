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



namespace hal::display
{
  inline const char* Domain = "hal.display";
  

  struct sess
  {
    point Drv;
    point Obj;
  };

  enum bufstate: u8
  {
    sFree    = 0,
    sInvalid = 1,
    sReady   = 2,
    sReading = 3,
  };

  enum format: u8
  {
    // 16
    iXRGB_4444  = 1,  /* uint 4:4:4:4 */
    iARGB_4444  = 2,  /* uint 4:4:4:4 */
    iXRGB_1555  = 3,  /* uint 1:5:5:5 */
    iARGB_1555  = 4,  /* uint 1:5:5:5 */

    // 24
    iRGB_888    = 5,  /* uint 8:8:8 */
    
    // 32
    iXRGB_8888  = 6,  /* uint 8:8:8:8 */
    iARGB_8888  = 7,  /* uint 8:8:8:8 */
    iXRGB_2AAA  = 8,  /* uint 2:10:10:10 */
    iARGB_2AAA  = 9,  /* uint 2:10:10:10 */

    // 64
    iXRGB_XXXX  = 10, /* uint  16:16:16:16 */
    iARGB_XXXX  = 11, /* uint  16:16:16:16 */
    fXRGB_XXXX  = 12, /* float 16:16:16:16 */
    fARGB_XXXX  = 13, /* float 16:16:16:16 */
  };

  struct screenMode
  {
    u16   Width;
    u16   Height;
    u32   Refresh;
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
      u16        (*ModeC)(__S);
      screenMode (*ModeG)(__S, u16 Index);

      // Connect
      bool  (*Connect)       (__S, u16 Mode, format Format, u8 BufCount);
      void  (*DisConnect)    (__S);
      bool  (*IsConnected)   (__S);
      bool  (*IsDevConnected)(__S);

      // Buffer
      u32    (*BufferSize)(__S);
      u8     (*BufferC)(__S);
      point  (*BufferG)(__S, u8 Index);

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

    gsDriver<display::sess> DRV;
  };

}
