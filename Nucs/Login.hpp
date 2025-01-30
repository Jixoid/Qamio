#pragma once

#include <iostream>

#include "JBasic.hpp"
#include "Nucleol.hpp"

using namespace std;

#define ComGet(X,Y) X = (Y)Com.Get(#X);




namespace jix {

  // Global
  

  // Window Control
  typedef void  (*login_ScrStart)();
  typedef void  (*login_ScrStop)();
  

  namespace Login {
    
    login_ScrStart  Login_ScrStart;
    login_ScrStop   Login_ScrStop;


    void PopNuc(sNucCom Com) {
      ComGet(Login_ScrStart, login_ScrStart);
      ComGet(Login_ScrStop,  login_ScrStop);
    }
  }


}
