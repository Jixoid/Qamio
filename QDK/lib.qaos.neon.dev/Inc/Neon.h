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

#include "Neon_Error.h"



#ifdef __cplusplus
extern "C" {
#endif


  // Types
  enum neon_contactType: u8
	{
		Host = 1,
    P2P  = 2,
	};

  struct neon_contact
  {
    neon_contactType Type;
    idA Aid;
  };

  typedef neon_contact* contact_p;


  typedef bool (*neon_mAccept)(idA Aid);
	typedef void (*neon_mHandler)(contact_p Con, data_ Data);



  // Methods
  contact_p neon_Connect(const char *ID);
  contact_p neon_Server (const char *ID, u32 ConCount);
  bool      neon_Close  (contact_p Con);

  bool    	neon_Send(contact_p Con, data_ Data);
  bool      neon_Recv(contact_p Con, data_* Data);

  bool      neon_Process(contact_p Con, bool *Work, neon_mAccept Accept, neon_mHandler Handler);


#ifdef __cplusplus
}
#endif
