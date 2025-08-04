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



namespace qaos::neon
{
  enum NeonError: u16
  {
    neOK       = 0,
    neUknown   = 1,
    neSocket   = 2,
    neConnect  = 3,
    neSend     = 4,
    neRecv     = 5,
    neMemory   = 6,
    neTimeout  = 7,
    neClosed   = 8,
    neInvParam = 9,
  };


  NeonError GetLastError();

  const char* ErrorStr(NeonError err);
}
