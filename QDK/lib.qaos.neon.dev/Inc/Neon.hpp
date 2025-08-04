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

#include "Neon_Error.hpp"


using namespace std;
using namespace jix;



namespace qaos::neon
{
  // Types
  enum contactType: u8
	{
		Host = 1,
    P2P  = 2,
	};

  struct contact
  {
    contactType Type;
    idA Aid;
  };

  using contact_p = contact*;


  using mAccept  = bool (*)(idA Aid);
	using mHandler = void (*)(contact_p Con, data_ Data);



  // Methods
  contact_p Connect(const char *ID);
  contact_p Server (const char *ID, u32 ConCount);
  bool      Close  (contact_p Con);

  bool    	Send(contact_p Con, data_ Data);
  bool      Recv(contact_p Con, data_ &Data);

  bool      Process(contact_p Con, bool *Work, mAccept Accept, mHandler Handler);

}
