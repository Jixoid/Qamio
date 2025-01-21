#pragma once

#include <iostream>

#include "JBasic.hpp"
#include "Nucleol.hpp"

using namespace std;
using namespace jix;


namespace jix::term {

  void Clear() {
    cout << "\033[2J\033[1;1H";
  }

  void Up() {
    cout << "\033[1A";
  }

}