#pragma once

#include <iostream>
#include <mutex>

#include "Basis.hpp"
#include "Nucleol.hpp"

#include "Widget.hpp"

using namespace std;
using namespace jix;



class __wPaint: public __wVisual {
public:
  // Basic
   __wPaint();
  ~__wPaint();
  
};


class __wPanel: public __wVisual {
public:
  // Basic
   __wPanel();
  ~__wPanel();


public:
  // Other
  void  Draw() override;

};


class __wButton: public __wVisual {
public:
  // Basic
   __wButton();
  ~__wButton();


public:
  // Other
  void  Draw() override;

};


class __wLabel: public __wVisual {
public:
  // Basic
   __wLabel();
  ~__wLabel();


public:
  // Other
  void  Draw() override;

};

