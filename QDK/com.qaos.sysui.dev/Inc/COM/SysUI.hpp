/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#pragma once

#include <iostream>

#include "Basis.h"
#include "Kernel.hpp"


using namespace std;
using namespace jix;



namespace com::sysui
{
  inline const char* Domain = "com.sysui";
  

  struct sDriver
  {
    void  (*Start)(idU UID);

  };
  
}
