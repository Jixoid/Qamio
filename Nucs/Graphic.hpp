#pragma once

#include <iostream>

#include "JBasic.hpp"
#include "Nucleol.hpp"

using namespace std;

#define ComGet(X,Y) X = (Y)Com.Get(#X);




namespace jix {

  // Global
  typedef point  surface2;
  typedef point  window2;

  struct rect
  {
    union {
      int32u Left;
      int32u L;
    };

    union {
      int32u Top;
      int32u T;
    };

    union {
      int32u Right;
      int32u R;
    };

    union {
      int32u Bottom;
      int32u B;
    };
  };
  
  typedef int32u  color;
  

  // Window Control
  typedef window2  (*window2_New)(uid Owner, int32u Width, int32u Height);
  typedef void     (*window2_Dis)(uid Owner, window2 Self);
  typedef void     (*window2_Swp)(uid Owner, window2 Self);
  typedef surface2 (*window2_Sur)(uid Owner, window2 Self);
  typedef surface2 (*window2_Buf)(uid Owner, window2 Self);

  typedef surface2 (*surface2_New)(uid Owner, int32u Width, int32u Height);
  typedef void     (*surface2_Dis)(uid Owner, surface2 Self);
  typedef point    (*surface2_Ass)(uid Owner, surface2 Self, point New);
  typedef void     (*surface2_RectF)(uid Owner, surface2 Self, rect Rect, color Color);


  namespace Graphic {
    
    window2_New  Window2_New;
    window2_Dis  Window2_Dis;
    window2_Swp  Window2_Swp;
    window2_Sur  Window2_Sur;
    window2_Buf  Window2_Buf;

    surface2_New  Surface2_New;
    surface2_Dis  Surface2_Dis;
    surface2_Ass  Surface2_Ass;
    surface2_RectF  Surface2_RectF;

    void PopNuc(sNucCom Com) {
      ComGet(Window2_New, window2_New);
      ComGet(Window2_Dis, window2_Dis);
      ComGet(Window2_Swp, window2_Swp);
      ComGet(Window2_Sur, window2_Sur);
      ComGet(Window2_Buf, window2_Buf);

      ComGet(Surface2_New, surface2_New);
      ComGet(Surface2_Dis, surface2_Dis);
      ComGet(Surface2_Ass, surface2_Ass);
      ComGet(Surface2_RectF, surface2_RectF);
    }
  }


}
