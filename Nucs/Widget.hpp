#pragma once

#include <iostream>

#include "JBasic.hpp"
#include "Nucleol.hpp"
#include "Graphic.hpp"

using namespace std;

#define NComGet(T,N) N = (T)Com.Get(#N);


namespace jix {

  // Global
  typedef  point   widget;
  typedef  point   wvisual; // : widget

  typedef  point   wbutton; // : wvisual : widget
  typedef  point   wpanel;  // : wvisual : widget
  


  // Widget
  typedef void     (*widget_Widget_Dis)     (widget Self);


  // Visual
  typedef point2d  (*widget_Visual_PointGet)(wvisual Self);
  typedef void     (*widget_Visual_PointSet)(wvisual Self, point2d Point);
  typedef size2d   (*widget_Visual_SizeGet) (wvisual Self);
  typedef void     (*widget_Visual_SizeSet) (wvisual Self, size2d Size);
  typedef surface2 (*widget_Visual_Buf)     (wvisual Self);
  typedef void     (*widget_Visual_Draw)    (wvisual Self);


  // Button
  typedef wbutton  (*widget_Button_New)    (widget Parent);

  // Panel
  typedef wpanel   (*widget_Panel_New)     (widget Parent);
  


  // Definition
  namespace Widget {
    
    // Widget
    widget_Widget_Dis  Widget_Widget_Dis;

    // Visual
    widget_Visual_PointGet  Widget_Visual_PointGet;
    widget_Visual_PointSet  Widget_Visual_PointSet;
    widget_Visual_SizeGet  Widget_Visual_SizeGet;
    widget_Visual_SizeSet  Widget_Visual_SizeSet;
    widget_Visual_Buf   Widget_Visual_Buf;
    widget_Visual_Draw  Widget_Visual_Draw;

    // Button
    widget_Button_New  Widget_Button_New;

    // Panel
    widget_Panel_New  Widget_Panel_New;


    void PopNuc(sNucCom Com) {

      // Widget
      NComGet(widget_Widget_Dis,  Widget_Widget_Dis);

      // Visual
      NComGet(widget_Visual_PointGet,  Widget_Visual_PointGet);
      NComGet(widget_Visual_PointSet,  Widget_Visual_PointSet);
      NComGet(widget_Visual_SizeGet,  Widget_Visual_SizeGet);
      NComGet(widget_Visual_SizeSet,  Widget_Visual_SizeSet);
      NComGet(widget_Visual_Buf,   Widget_Visual_Buf);
      NComGet(widget_Visual_Draw,  Widget_Visual_Draw);

      // Button
      NComGet(widget_Button_New,  Widget_Button_New);

      // Panel
      NComGet(widget_Panel_New,  Widget_Panel_New);
    }
  }

}

#undef NComGet
