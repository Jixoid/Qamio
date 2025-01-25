#pragma once

#include <iostream>

#include "JBasic.hpp"
#include "Nucleol.hpp"

using namespace std;

#define ComGet(X,Y) X = (Y)Com.Get(#X);




namespace jix {

  // Global
  typedef point  window2;

  struct rect
  {
    int32u Left, Top;
    int32u Right, Bottom;
  };
  
  typedef int32u  color;
  

  // Window Control
  typedef window2 (*window2_New)(uid Owner, int32u Width, int32u Height);
  typedef void    (*window2_Dis)(window2 Self);
  typedef void    (*window2_Swp)(window2 Self);
  typedef point   (*window2_Buf)(window2 Self);
  typedef void    (*window2_RectF)(window2 Self, rect Rect, color Color);


  namespace Graphic {
    
    window2_New  Window2_New;
    window2_Dis  Window2_Dis;
    window2_Swp  Window2_Swp;
    window2_Buf  Window2_Buf;
    window2_RectF Window2_RectF;

    void PopNuc(sNucCom Com) {
      ComGet(Window2_New, window2_New);
      ComGet(Window2_Dis, window2_Dis);
      ComGet(Window2_Swp, window2_Swp);
      ComGet(Window2_Buf, window2_Buf);
      ComGet(Window2_RectF, window2_RectF);
    }
  }


}
