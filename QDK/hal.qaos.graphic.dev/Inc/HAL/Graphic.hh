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



namespace hal::graphic
{
  inline const char* Domain = "hal.graphic";
  

  struct sess
  {
    point Drv;
    point Obj;
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



  struct color
  {
    f32 A,R,G,B;

    color() = default;

    explicit color(f32 nR, f32 nG, f32 nB, f32 nA = 1)
    {
      A = nA;
      R = nR;
      G = nG;
      B = nB;
    }
    
    explicit color(u32 Col)
    {
      A = ((Col >> 24) & 0xFF) /255.0f;
      R = ((Col >> 16) & 0xFF) /255.0f;
      G = ((Col >> 8)  & 0xFF) /255.0f;
      B = ((Col >> 0)  & 0xFF) /255.0f;
    }

    operator u32() const
    {
      return 
        (u32(A*255) << 24) |
        (u32(R*255) << 16) |
        (u32(G*255) << 8) |
        (u32(B*255) << 0);
    }

  };

  struct point2d
  {
    f32 X,Y;
  };

  struct size2d
  {
    f32 W,H;
  };

  struct rect2d
  {
    f32 X,Y,W,H;
  };


  template <typename __S>
  struct gsDriver
  {
    // New / Dis
    __S      (*New)(u32 Index, u16 Width, u16 Height);
    void     (*Dis)(__S);

    // New With
    __S      (*New_With_Mem)(u32 Index, u16 Width, u16 Height, point Buffer);
    __S      (*New_With_Sur)(u32 Index, __S Sample);
    __S      (*New_With_Png)(u32 Index, const char* Path);
    __S      (*New_With_Bmp)(u32 Index, const char* Path);

    // API
    color    (*Pixel)  (__S, point2d Point);
    void     (*Clear)  (__S);

    // Size
    size2d   (*SizeGet)(__S);
    void     (*SizeSet)(__S, u16 Width, u16 Height);
    
    // Pre Config
    void     (*Set_Source_RGB) (__S, color Color);
    void     (*Set_Source_RGBA)(__S, color Color);
    void     (*Set_Source_Sur) (__S, __S Src, point2d Point);

    void     (*Set_Font_Size)  (__S, f32 Size);


    // Basis
    void     (*Basis_Move)(__S, point2d Point);

    void     (*Basis_Line)(__S, point2d Point);
    void     (*Basis_Rect)(__S, rect2d Rect);
    void     (*Basis_Arc) (__S, point2d Point, f32 Round, f32 Angle1, f32 Angle2);

    void     (*Basis_RectR)(__S, rect2d Rect, f32 Round);
    

    // Text
    size2d   (*Text_Size)(__S, const char* Text);  
    void     (*Text)     (__S, const char* Text);
    
    
    // OK
    void     (*Flush)  (__S);
    void     (*Paint)  (__S);
    void     (*Paint_A)(__S, f32 Alpha);
    void     (*Fill)   (__S);
    void     (*Stroke) (__S);
  };

  using sDriver = gsDriver<point>;

  struct sHAL
  {
    bool  (*RegDriver)(sDriver *Driver);
    bool  (*DelDriver)(sDriver *Driver);

    u32   (*Count)();

    gsDriver<graphic::sess> DRV;
  };

}
