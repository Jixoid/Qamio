#pragma once

#include <iostream>
#include <fstream>
#include <vector>

#include "Basis.hpp"
#include "Nucleol.hpp"


using namespace std;
using namespace jix;




class cNucMng {
protected:

  struct sNucleol
  {
    string  Package;
    string  Part;
    point   Handle;
    sNucStd *Funcs;
  };

  vector<sNucleol> Nucleols;


public:
  void GetNucList();
  void LoadModules();

  void Check();

  void Push(sNucCom Com);
  void Pop();

  void Load();
  void Unload();

  void Start();
  void Stop();
};
