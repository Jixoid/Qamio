/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#pragma once

#include <stdio.h>

#include "Basis.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef u16 neon_error;
enum neon_errors
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


neon_error neon_GetLastError();

const char* neon_ErrorStr(neon_error err);


#ifdef __cplusplus
}
#endif