/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#include <iostream>

#include "Basis.h"
#include "Kernel.hpp"

#include "Neon_Error.hpp"


using namespace std;
using namespace jix;



namespace qaos::neon
{
  // Thread-local değişken
  __thread NeonError LastError = NeonError::neOK;

  
  NeonError GetLastError()
  {
    return LastError;
  }

  const char* ErrorStr(NeonError Err)
  {
    switch (Err)
    {
      case NeonError::neOK:        return "Başarılı";
      case NeonError::neUknown:    return "Bilinmeyen hata";
      case NeonError::neSocket:    return "Socket oluşturulamadı";
      case NeonError::neConnect:   return "Bağlantı kurulamadı";
      case NeonError::neSend:      return "Veri gönderilemedi";
      case NeonError::neRecv:      return "Veri alınamadı";
      case NeonError::neMemory:    return "Bellek yetersiz";
      case NeonError::neTimeout:   return "Zaman aşımı";
      case NeonError::neClosed:    return "Soket kapatılmış";
      case NeonError::neInvParam:  return "Geçersiz parametre";
      
      default: return "Tanımsız hata";
    }
  }

}
