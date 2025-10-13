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


  template<typename _t>
  struct poit2
  {
    _t X,Y;
  };

  using poit2_f32 = poit2<f32>;
  using poit2_i32 = poit2<i32>;

  template<typename _t>
  struct size2
  {
    _t W,H;
  };

  using size2_f32 = size2<f32>;
  using size2_i32 = size2<i32>;
  using size2_u32 = size2<u32>;


  template<typename _t>
  struct rect2
  {
    _t
      X1,Y1,
      X2,Y2;
  };

  using rect2_f32 = rect2<f32>;
  using rect2_i32 = rect2<i32>;


  template <typename __S>
  struct gsDriver
  {
    // New / Dis
    __S  (*New)(u32 Index, u16 Width, u16 Height);
    void (*Dis)(__S);

    // New With
    __S (*New_With_Mem)(u32 Index, u16 Width, u16 Height, point Buffer);
    __S (*New_With_Sur)(u32 Index, __S Sample);
    __S (*New_With_Png)(u32 Index, const char* Path);
    __S (*New_With_Bmp)(u32 Index, const char* Path);

    // Pre Config
    void (*Set_Color)  (__S, color Color);
    void (*Set_Source) (__S, __S Src);
    void (*Set_FontSize)  (__S, f32 Size);
    void (*Set_LineSize)  (__S, f32 Size);

    // Func
    void (*Get_Size)(__S, size2_i32 *Ret);
    void (*Set_Size)(__S, u16 Width, u16 Height);
    void (*Set_Pos) (__S, poit2_f32 Val);
    void (*Set_rPos)(__S, poit2_f32 Val);


    // Draw
    void (*Draw_Rect)(__S, rect2_f32 Rect);
    void (*Draw_RectRound)(__S, rect2_f32 Rect, f32 Radius);
    void (*Draw_RectRound4)(__S, rect2_f32 Rect, f32 R_LT, f32 R_LB, f32 R_RT, f32 R_RB);
    void (*Draw_Line)(__S, poit2_f32 P1, poit2_f32 P2);
    void (*Draw_ToLine)(__S, poit2_f32 P);
    void (*Draw_Arc)(__S, poit2_f32 Point, f32 Radius, f32 Ang1, f32 Ang2);
    void (*Draw_Text)(__S, const char* Text);

    void (*Calc_Text)(__S, const char* Text, size2_f32 *Ret);  

    
    // API
    void  (*Stroke) (__S);
    void  (*Fill)   (__S);
    void  (*Paint)  (__S);
    void  (*Paint_A)(__S, f32 Alpha);
    void  (*Clip)   (__S);
    void  (*Clip_Reset)(__S);
    
    color (*Pixel)  (__S, poit2_i32 Point);
    void  (*Clear)  (__S);
    void  (*Flush)  (__S);
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
