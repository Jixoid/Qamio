#pragma once

#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;


#include "Basis.hpp"


namespace jix::JConf {

  extern
  const char *_c_Magic;

  const i8 _c_Val = 0x1;
  const i8 _c_Stc = 0x2;
  const i8 _c_Arr = 0x3;
  

  class cObj {
    public:
      virtual ~cObj() {}

    public:
      string GetVal();

      unordered_map<string, cObj*> GetStc();

      vector<cObj*> GetArr();

    };
  
  class cVal: public cObj {
  public:
    string Value;
  };

  class cStc: public cObj {
  public:
    unordered_map<string, cObj*> Objs;

  public:
    ~cStc() {
      for (auto &It: Objs)
        delete It.second;
    }
  };
  
  class cArr: public cObj {
  public:
    vector<cObj*> Objs;

  public:
    ~cArr() {
      for (auto &Obj: Objs)
        delete Obj;
    }
  };
  

  cStc* ParseRaw(string FPath);
  void  WriteRaw(string FPath, cStc *Data);

  cStc* ParseBin(string FPath);
  void  WriteBin(string FPath, cStc *Data);

}
