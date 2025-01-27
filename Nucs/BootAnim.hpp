#pragma once

#include <iostream>

#include "JBasic.hpp"
#include "Nucleol.hpp"

using namespace std;

#define ComGet(X,Y) X = (Y)Com.Get(#X);




namespace jix {

  // Global


  // Window Control
  typedef void  (*bootAnim_Start)();
  typedef void  (*bootAnim_Stop)();
  typedef void  (*bootAnim_Prog)(int16u Max, int16u Val);
  

  namespace BootAnim {
    
    bootAnim_Start  BootAnim_Start;
    bootAnim_Stop   BootAnim_Stop;
    bootAnim_Prog   BootAnim_Prog;

    void PopNuc(sNucCom Com) {
      ComGet(BootAnim_Start, bootAnim_Start);
      ComGet(BootAnim_Stop,  bootAnim_Stop);
      ComGet(BootAnim_Prog,  bootAnim_Prog);
    }
  }


}
