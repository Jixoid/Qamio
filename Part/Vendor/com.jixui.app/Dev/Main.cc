/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#include <iostream>

#include "Basis.h"
#include "Neon.h"
#include "FWK/Argon.hh"


using namespace std;
using namespace jix;


contact_p NSysUI;


ohid Window_New(u32 W, u32 H, u32 F = 0)
{
  fwk::argon::WIN_New CMD = {
    .OpCode =  fwk::argon::commands::acNew,
    .W = W,
    .H = H,
    .Flags = F,
  };
  data_ Data = {&CMD, sizeof(CMD)};


  neon_Send(NSysUI, Data);

  return 0;
}


int main()
{
  NSysUI = neon_Connect("com.qaos.sysui");
  if (NSysUI == Nil)
  {
    cerr << "Not Connected" << endl;
    return 1;
  }

  Window_New(400,300);
}
