#define ef else if


#include <iostream>
#include <filesystem>
#include <thread>
#include <atomic>

#include "Basis.hpp"

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "Neon.hpp"

using namespace std;
using namespace jix;



namespace Neon
{
	string Neonization(string ID)
	{
		return "/Neon/" +ID;
	}
	


	// Connect
		contact Connect_Socket(string &ID, string Path);

	contact Connect(string ID)
	{
		auto Obj = Contacts.find(ID);
    if (Obj != Contacts.end())
      throw runtime_error("@Neon: Contact already found: " +ID);


		// File exists ?
		if (filesystem::is_socket(Neonization(ID)))
			return Connect_Socket(ID, Neonization(ID));
		
		else
			throw runtime_error("@Neon: Not found: " +ID);

	}

	inline contact Connect_Socket(string &ID, string Path)
	{
		// Interface
		int Socket;


		// Soketi oluştur
		Socket = socket(AF_UNIX, SOCK_STREAM, 0);
		if (Socket == -1)
			throw runtime_error("@Neon: Socket can not create: " +ID);


		// Sunucu adresini belirle
		struct sockaddr_un ServerAddr;

		memset(&ServerAddr, 0, sizeof(ServerAddr));
		ServerAddr.sun_family = AF_UNIX;
		strcpy(ServerAddr.sun_path, Path.c_str());


		// Sunucuya bağlan
		if (connect(Socket, (struct sockaddr*)&ServerAddr, sizeof(ServerAddr)) == -1)
		{
			close(Socket);
			throw runtime_error("@Neon: Can not connected: " +ID);
		}

		
		// Get Infos
		idU Uid;

		struct ucred cred;
		socklen_t len = sizeof(struct ucred);
		if (getsockopt(Socket, SOL_SOCKET, SO_PEERCRED, &cred, &len) == 0)
			Uid = cred.uid;
		else
		{
			close(Socket);
			throw runtime_error("@Neon: Can't get host information");
		}


		// Kişilere ekle
		contact Ret = {
			.Type    = contactType::P2P,
			.Socket  = Socket,
			.Uid     = Uid,
		};

		Contacts[ID] = Ret;
		return Ret;
	}



	// Server
		contact Server_Socket(string ID);

	contact Server(string ID)
	{
		return Server_Socket(ID);
	}

	inline contact Server_Socket(string ID)
	{
		// Interface
		int Socket;


		// Soketi oluştur
		Socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (Socket == -1)
			throw runtime_error("@Neon: Socket can not create: " +ID);
			
		
		// Sunucu adresini belirle
		struct sockaddr_un ServerAddr;

		memset(&ServerAddr, 0, sizeof(ServerAddr));
		ServerAddr.sun_family = AF_UNIX;
		strcpy(ServerAddr.sun_path, Neonization(ID).c_str());


		// Soketi belirtilen adrese bağla
    if (bind(Socket, (struct sockaddr*)&ServerAddr, sizeof(ServerAddr)) == -1)
		{
			close(Socket);
			throw runtime_error("@Neon: Can not connected: " +ID);
		}

		
		// Dinlemeye başla
    if (listen(Socket, 100) == -1)
		{
			close(Socket);
			throw runtime_error("@Neon: Can not listening: " +ID);
		}


		// Kişilere ekle
		contact Ret = {
			.Type    = contactType::Host,
			.Socket  = Socket,
		};

		Contacts[ID] = Ret;
		return Ret;
	}



	// Close
		void Close_Socket(contact Con);

	void Close(string ID)
	{
		auto Obj = Contacts.find(ID);
    if (Obj == Contacts.end())
      throw runtime_error("@Neon: Contact not found: " +ID);


		Close_Socket(Obj->second);


		Contacts.erase(ID);
	}

	inline void Close_Socket(contact Con)
	{
		close(Con.Socket);
	}



	// Process
	void MessageHandler(contact *Con, atomic<bool> *Work, mHandler Handler)
	{
		while (Work->load())
		{
			uPtr Bytes;
			sData Data;

			// Read Size
			Bytes = recv(Con->Socket, &Data.Size, sizeof(Data.Size), 0);
			if (Bytes == -1)
				throw runtime_error("@Neon: Socket not readed");
			ef (Bytes == 0)
			{
				clog << "Connection closed" << endl;
				break;
			}

			Data.Point = malloc(Data.Size);
		

			// Read Data
			Bytes = recv(Con->Socket, Data.Point, Data.Size, 0);
			if (Bytes == -1)
				throw runtime_error("@Neon: Socket not readed");
			ef (Bytes == 0)
			{
				clog << "Connection closed" << endl;
				free(Data.Point);
				break;
			}


			Handler(*Con, Data);


			free(Data.Point);
		}


		// Close
		close(Con->Socket);
	}


	void Process(contact Con, atomic<bool> *Work, mAccept Accept, mHandler Handler)
	{
		// Will be destroyed
		vector<contact*> TCon;
		vector<thread*> TThread;


		// Accept loop
		while (Work->load())
		{
			// Interface
			int SubSocket;
			idU Uid;


			// Accept
			struct sockaddr_un Addr;
			u32 Size = sizeof(Addr);

			SubSocket = accept(Con.Socket, (struct sockaddr *)&Addr, &Size);
			if (SubSocket == -1)
				throw runtime_error("@Neon: Connection refused");



			// Get Infos
			struct ucred cred;
			socklen_t len = sizeof(struct ucred);
			if (getsockopt(SubSocket, SOL_SOCKET, SO_PEERCRED, &cred, &len) == 0)
				Uid = cred.uid;
			else
			{
				close(SubSocket);
				throw runtime_error("@Neon: Can't get client information");
			}


			// Accept
			if (Accept != Nil)
				if (Accept(Uid) == false)
				{
					clog << "Connection refused" << endl;

					close(SubSocket);
					continue;
				}
			

			clog << "New connection has been established" << endl;


			// Kişilere ekle
			contact *Ret = new contact {
				.Type    = contactType::P2P,
				.Socket  = SubSocket,
				.Uid     = Uid,
			};
			TCon.push_back(Ret);


			// New Thread
			thread *AThread = new thread(&MessageHandler, Ret, Work, Handler);
			TThread.push_back(AThread);
		}


		// Destroy trashs
		for (auto A: TCon)    delete A;
		for (auto A: TThread) delete A;

	}



	// Send
		void Send_Socket(contact Con, sData Data);

	void Send(string ID, sData Data)
	{
		auto Obj = Contacts.find(ID);
    if (Obj == Contacts.end())
      throw runtime_error("@Neon: Contact not found: " +ID);


		Send(Obj->second, Data);
	}

	void Send(contact Con, sData Data)
	{
		if (Con.Type == contactType::Host)
			throw runtime_error("@Neon: Can't be sent to this socket");

		Send_Socket(Con, Data);
	}

	inline void Send_Socket(contact Con, sData Data)
	{
		if (send(Con.Socket, &Data.Size, sizeof(uPtr), 0) == -1)
			throw runtime_error("@Neon: Message could not be delivered");
			

		if (send(Con.Socket, Data.Point, Data.Size, 0) == -1)
			throw runtime_error("@Neon: Message could not be delivered");
	}



	// Recv
		void Recv_Socket(contact Con, sData *Data);

	void Recv(string  ID, sData *Data)
	{
		auto Obj = Contacts.find(ID);
    if (Obj == Contacts.end())
      throw runtime_error("@Neon: Contact not found: " +ID);


		Recv(Obj->second, Data);
	}

	void Recv(contact Con, sData *Data)
	{
		if (Con.Type == contactType::Host)
			throw runtime_error("@Neon: Can't be recv from this socket");

		Recv_Socket(Con, Data);
	}

	inline void Recv_Socket(contact Con, sData *Data)
	{
		uPtr Bytes;

		// Read Size
		Bytes = recv(Con.Socket, &Data->Size, sizeof(Data->Size), 0);
		if (Bytes == -1)
			throw runtime_error("@Neon: Socket not readed");
		ef (Bytes == 0)
			throw runtime_error("@Neon: Socket is closed");

		Data->Point = malloc(Data->Size);
	

		// Read Data
		Bytes = recv(Con.Socket, Data->Point, Data->Size, 0);
		if (Bytes == -1)
			throw runtime_error("@Neon: Socket not readed");
		ef (Bytes == 0)
			throw runtime_error("@Neon: Socket is closed");

	}

}
