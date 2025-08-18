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
#include "Kernel.hh"


using namespace std;
using namespace jix;



namespace com::user
{
  inline const char* Domain = "com.user";
  

  struct sDriver
  {
    u32   (*UserC)();
    idU   (*UserG)(u32 Index);

    char* (*GetName)(idU UID); /* with kernel::alloc() */
    char* (*GetNick)(idU UID); /* with kernel::alloc() */

    bool  (*Pass)(idU UID, const char* Key);
  };
  
}
