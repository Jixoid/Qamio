#pragma once

#include <iostream>

#include "JBasic.hpp"
#include "Nucleol.hpp"

using namespace std;

#define NComGet(T,N) N = (T)Com.Get(#N);


namespace jix {

  // Global
  typedef point  surface2;
  typedef point  window2;

  struct rect2d {

    union {
      int32f L, Left;
    };

    union {
      int32f T, Top;
    };

    union {
      int32f W, Width;
    };

    union {
      int32f H, Height;
    };
  };
  
  struct size2d {

    union {
      int32f W, Width;
    };

    union {
      int32f H, Height;
    };
  };

  struct point2d {

    int32f X, Y;
  };


  struct color {
    
    union {
      int32f A, Alpha;
    };
    
    union {
      int32f R, Red;
    };

    union {
      int32f G, Green;
    };
    
    union {
      int32f B, Blue;
    };

    color() = default;

    explicit color(int32f nR, int32f nG, int32f nB, int32f nA = 0) {
      A = nA;
      R = nR;
      G = nG;
      B = nB;
    }
    
    explicit color(int32u Col) {
      A = ((Col >> 24) & 0xFF) /255.0f;
      R = ((Col >> 16) & 0xFF) /255.0f;
      G = ((Col >> 8)  & 0xFF) /255.0f;
      B = ((Col >> 0)  & 0xFF) /255.0f;
    }

    operator int32u() const {
      return 
        (int32u(A*255) << 24) |
        (int32u(R*255) << 16) |
        (int32u(G*255) << 8) |
        (int32u(B*255) << 0);
    }

  };


  // Surface Control
  typedef surface2 (*surface2_New)    (int32u Width, int32u Height);
  typedef surface2 (*surface2_NewWith)(int32u Width, int32u Height, point Data);
  typedef void     (*surface2_Dis)    (surface2 Self);
  typedef void     (*surface2_DisWith)(surface2 Self);
  typedef color    (*surface2_Get)    (surface2 Self, point2d Point);
  typedef size2d   (*surface2_SizeGet)(surface2 Self);
  typedef void     (*surface2_SizeSet)(surface2 Self, size2d Size);

  typedef void     (*surface2_Draw_Sur2)(surface2 Self, surface2 Src, point2d Point);
  typedef void     (*surface2_Draw_bmp) (surface2 Self, string Path, point2d Point);
  
  typedef void     (*surface2_RectF)(surface2 Self, rect2d Rect, color Color);
  typedef void     (*surface2_RectB)(surface2 Self, rect2d Rect, color Color, int16u Width);


  // Window Control
  typedef window2  (*window2_New)    (int32u Width, int32u Height);
  typedef window2  (*window2_NewWith)(int32u Width, int32u Height, point Data1, point Data2);
  typedef void     (*window2_Dis)    (window2 Self);
  typedef void     (*window2_DisWith)(window2 Self);
  typedef void     (*window2_Swp)    (window2 Self);
  typedef surface2 (*window2_Sur)    ( window2 Self);
  typedef surface2 (*window2_Buf)    (window2 Self);
  typedef size2d   (*window2_SizeGet)(window2 Self);
  typedef void     (*window2_SizeSet)(window2 Self, size2d Size);



  namespace Graphic {

    // Surface
    surface2_New      Surface2_New;
    surface2_NewWith  Surface2_NewWith;
    surface2_Dis      Surface2_Dis;
    surface2_DisWith  Surface2_DisWith;
    surface2_Get      Surface2_Get;
    surface2_SizeGet  Surface2_SizeGet;
    surface2_SizeSet  Surface2_SizeSet;

    surface2_Draw_Sur2 Surface2_Draw_Sur2;
    surface2_Draw_bmp  Surface2_Draw_bmp;

    surface2_RectF  Surface2_RectF;
    surface2_RectB  Surface2_RectB;


    // Window
    window2_New      Window2_New;
    window2_NewWith  Window2_NewWith;
    window2_Dis      Window2_Dis;
    window2_DisWith  Window2_DisWith;
    window2_Swp      Window2_Swp;
    window2_Sur      Window2_Sur;
    window2_Buf      Window2_Buf;
    window2_SizeGet  Window2_SizeGet;
    window2_SizeSet  Window2_SizeSet;


    void PopNuc(sNucCom Com) {

      // Surface
      NComGet(surface2_New,      Surface2_New);
      NComGet(surface2_NewWith,  Surface2_NewWith);
      NComGet(surface2_Dis,      Surface2_Dis);
      NComGet(surface2_DisWith,  Surface2_DisWith);
      NComGet(surface2_Get,      Surface2_Get);
      NComGet(surface2_SizeGet,  Surface2_SizeGet);
      NComGet(surface2_SizeSet,  Surface2_SizeSet);

      NComGet(surface2_Draw_Sur2, Surface2_Draw_Sur2);
      NComGet(surface2_Draw_bmp,  Surface2_Draw_bmp);

      NComGet(surface2_RectF,  Surface2_RectF);
      NComGet(surface2_RectB,  Surface2_RectB);


      // Window
      NComGet(window2_New,      Window2_New);
      NComGet(window2_NewWith,  Window2_NewWith);
      NComGet(window2_Dis,      Window2_Dis);
      NComGet(window2_DisWith,  Window2_DisWith);
      NComGet(window2_Swp,      Window2_Swp);
      NComGet(window2_Sur,      Window2_Sur);
      NComGet(window2_Buf,      Window2_Buf);
      NComGet(window2_SizeGet,  Window2_SizeGet);
      NComGet(window2_SizeSet,  Window2_SizeSet);
    }
  }

}

#undef NComGet
