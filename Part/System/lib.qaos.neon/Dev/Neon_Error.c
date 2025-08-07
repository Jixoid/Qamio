/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#include <stdio.h>

#include "Basis.h"

#include "Neon_Error.h"


#ifdef __cplusplus
extern "C" {
#endif

  // Thread-local değişken
  __thread neon_error LastError = neOK;

  
  neon_error neon_GetLastError()
  {
    return LastError;
  }

  const char* neon_ErrorStr(neon_error Err)
  {
    switch (Err)
    {
      case neOK:        return "Başarılı";
      case neUknown:    return "Bilinmeyen hata";
      case neSocket:    return "Socket oluşturulamadı";
      case neConnect:   return "Bağlantı kurulamadı";
      case neSend:      return "Veri gönderilemedi";
      case neRecv:      return "Veri alınamadı";
      case neMemory:    return "Bellek yetersiz";
      case neTimeout:   return "Zaman aşımı";
      case neClosed:    return "Soket kapatılmış";
      case neInvParam:  return "Geçersiz parametre";
      
      default: return "Tanımsız hata";
    }
  }

#ifdef __cplusplus
}
#endif
