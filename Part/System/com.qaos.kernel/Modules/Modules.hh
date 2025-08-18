/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#pragma once

#include <iostream>
#include <fstream>
#include <vector>

#include "Basis.h"
#include "Kernel.hh"


using namespace std;
using namespace jix;

using namespace qaos;




class cNucMng {
protected:

  struct sNucleol
  {
    string  Package;
    string  Part;
    point   Handle;
    kernel::nucStd *Funcs;
    bool Detached = false;
  };

public:
  vector<sNucleol> Nucleols;


public:
  void GetNucList();
  void LoadModules();
  void UnloadModules();

  // Ver
  void Check();

  // Load / Unload
  void Load();
  void Unload();

  // Start / Stop
  void Start();
  void Stop();

  // Push / Pop
  void Push();
  void Pop();

  void Push_Drv();
};
