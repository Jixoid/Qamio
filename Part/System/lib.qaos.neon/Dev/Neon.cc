/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#define ef else if
#define el else

#include <iostream>
#include <vector>
#include <filesystem>
#include <tuple>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "Basis.h"

#include "Neon.h"

#include "Neon_Error.h"


using namespace std;
namespace fs = std::filesystem; 
using namespace jix;



extern __thread neon_error LastError;



// Macro
#define Neonization(X) ("/Neon/"+string(X))



// Types
struct __contact: neon_contact
{
  int Socket;

  shared_mutex Locker;
  vector<tuple<neon_contact*, thread*, bool*>> ConList;
};



// Methods
#define  Self ((__contact*)__Self)

extern "C" contact_p neon_Connect(const char *ID)
{
  // Interface
  __contact *Ret = new __contact();
  Ret->Type = neon_contactType::P2P;

  string Path = Neonization(ID);
  

  // FileStat
  if (!fs::is_socket(Path))
  {
    LastError = neon_errors::neSocket; // Target socket not found
    return Nil;
  }

  // Soketi oluştur
  Ret->Socket = socket(AF_UNIX, SOCK_STREAM, 0);
  if (Ret->Socket == -1)
  {
    LastError = neon_errors::neSocket; // Socket can not create
    return Nil;
  }

  // Sunucu adresini belirle
  struct sockaddr_un ServerAddr;

  memset(&ServerAddr, 0, sizeof(ServerAddr));
  ServerAddr.sun_family = AF_UNIX;
  strcpy(ServerAddr.sun_path, Path.c_str());


  // Sunucuya bağlan
  if (connect(Ret->Socket, (struct sockaddr*)&ServerAddr, sizeof(ServerAddr)) == -1)
  {
    close(Ret->Socket);
    LastError = neon_errors::neConnect; // Can not connected
    return Nil;
  }


  // Get Infos
  struct ucred cred;
  socklen_t len = sizeof(struct ucred);
  if (getsockopt(Ret->Socket, SOL_SOCKET, SO_PEERCRED, &cred, &len) == 0)
    Ret->Aid = {
      .Pid = cred.pid,
      .Uid = cred.uid,
      .Gid = cred.gid,
    };
  else
  {
    close(Ret->Socket);
    LastError = neon_errors::neConnect; // Can't get host information
    return Nil;
  }
  

  clog << "@Neon: Connect: " << ID << endl;

  LastError = neon_errors::neOK;
  return Ret;
}

extern "C" contact_p neon_Server(const char *ID, u32 ConnectCount)
{
    // Interface
  __contact *Ret = new __contact();
  Ret->Type = neon_contactType::Host;

  string Path = Neonization(ID);

  
  // Soketi oluştur
  Ret->Socket = socket(AF_UNIX, SOCK_STREAM, 0);
  if (Ret->Socket == -1)
  {
    LastError = neon_errors::neSocket; // Socket can not create
    return Nil;
  }	
  
  // Sunucu adresini belirle
  struct sockaddr_un ServerAddr;

  memset(&ServerAddr, 0, sizeof(ServerAddr));
  ServerAddr.sun_family = AF_UNIX;
  strcpy(ServerAddr.sun_path, Neonization(ID).c_str());


  // Soketi belirtilen adrese bağla
  if (bind(Ret->Socket, (struct sockaddr*)&ServerAddr, sizeof(ServerAddr)) == -1)
  {
    close(Ret->Socket);
    LastError = neon_errors::neConnect; // Can not connected
    return Nil;
  }

  
  // Dinlemeye başla
  if (listen(Ret->Socket, ConnectCount) == -1)
  {
    close(Ret->Socket);
    LastError = neon_errors::neConnect; // Can not listening
    return Nil;
  }


  clog << "@Neon: Server: " << ID << endl;

  LastError = neon_errors::neOK;
  return Ret;
}

extern "C" bool      neon_Close(contact_p __Self)
{
  close(Self->Socket);


  if (Self->Type == neon_contactType::Host)
    for (auto &[Con, The, FW]: Self->ConList)
    {
      // Fork-Work
      *FW = false;

      // Thread
      if (The->joinable())
        The->join();

      delete The;
      delete FW;

      // Contact
      neon_Close(Con);
    }


  delete Self;

  return true;
}

#undef  Ret



#define  Con ((__contact*)__Con)
//@ Not export
void MessageHandler(contact_p __Con, bool *Work, neon_mHandler Handler)
{
  while (*Work)
  {
    data_ Data;
    if (!neon_Recv(__Con, &Data))
      continue;

    Handler(__Con, Data);

    free(Data.Point);
  }

  // Close
  close(Con->Socket);
}



extern "C" bool     neon_Send(contact_p __Con, data_ Data)
{
  if (send(Con->Socket, &Data.Size, sizeof(Data.Size), 0) == -1)
  {
    LastError = neon_errors::neSend; // Message could not be delivered
    return false;
  }

  if (send(Con->Socket, Data.Point, Data.Size, 0) == -1)
  {
    LastError = neon_errors::neSend; // Message could not be delivered
    return false;
  }


  return true;
}

extern "C" bool     neon_Recv(contact_p __Con, data_ *Data)
{
  decltype(Data->Size) Bytes;

  
  // Read Size
  Bytes = recv(Con->Socket, &Data->Size, sizeof(Data->Size), 0);
  if (Bytes == -1)
  {
    LastError = neon_errors::neRecv; // Socket not readed
    return false;
  }

  ef (Bytes == 0)
  {
    LastError = neon_errors::neClosed; // Socket is closed
    return false;
  }

  Data->Point = malloc(Data->Size);
  if (Data->Point == Nil)
  {
    LastError = neon_errors::neMemory;
    return false;
  }


  // Read Data
  Bytes = recv(Con->Socket, Data->Point, Data->Size, 0);
  if (Bytes == -1)
  {
    free(Data->Point);
    LastError = neon_errors::neRecv; // Socket not readed
    return false;
  }

  ef (Bytes == 0)
  {
    free(Data->Point);
    LastError = neon_errors::neClosed; // Socket is closed
    return false;
  }


  return true;
}


extern "C" bool     neon_Process(contact_p __Con, bool *Work, neon_mAccept Accept, neon_mHandler Handler)
{
  // Accept loop
  while (*Work)
  {
    // Accept
    struct sockaddr_un Addr;
    u32 Size = sizeof(Addr);

    int SubSocket = accept(Con->Socket, (struct sockaddr *)&Addr, &Size);
    if (SubSocket == -1)
    {
      LastError = neon_errors::neConnect;  // Connection refused
      return false;
    }

    // Get Infos
    idA Aid;
    struct ucred cred;
    socklen_t len = sizeof(struct ucred);
    if (getsockopt(SubSocket, SOL_SOCKET, SO_PEERCRED, &cred, &len) == 0)
      Aid = {
        .Pid = cred.pid,
        .Uid = cred.uid,
        .Gid = cred.gid,
      };
    
    el
    {
      close(SubSocket);
      LastError = neon_errors::neConnect; // Can't get client information
      return false;
    }


    // Accept
    if (Accept != Nil)
      if (Accept(Aid) == false)
      {
        clog << "@Neon: Connection refused" << endl;

        close(SubSocket);
        continue;
      }
    

    clog << "@Neon: New connection has been established" << endl;


    // Yeni Bağlantı
    {
      unique_lock Lock(Con->Locker);


      // Yeni Bool
      bool *FWork = new bool(true);
      

      // Listeye ekle
      __contact *ACon = new __contact();
      ACon->Socket = SubSocket;
      ACon->Type   = neon_contactType::P2P;
      ACon->Aid    = Aid;
      

      // New Thread
      thread *AThread = new thread(&MessageHandler, ACon, FWork, Handler);
      

      Con->ConList.push_back({ ACon, AThread, FWork });
    }

  }


  return true;
}

#undef Con
