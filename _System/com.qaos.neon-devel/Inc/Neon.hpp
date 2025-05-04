#pragma once

#include <iostream>
#include <vector>
#include <atomic>
#include <unordered_map>

#include "Basis.hpp"

using namespace std;
using namespace jix;



namespace Neon
{
	// Types
	enum contactType
	{
		P2P,
		Host,
	};


	struct contact
	{
		contactType Type;
		int Socket;


		// Connection Info
		idU Uid;
	};

	typedef bool (*mAccept)(idU Uid);
	typedef void (*mHandler)(contact Con, sData Data);



	// APIs
	unordered_map<string, contact> Contacts;


	contact Connect(string ID);
	contact Server(string ID);
	void    Close(string ID);

	void    Process(contact Con, atomic<bool> *Work, mAccept Accept, mHandler Handler);

	void 		Send(string  ID,  sData Data);
	void 		Send(contact Con, sData Data);

	void    Recv(string  ID,  sData *Data);
	void    Recv(contact Con, sData *Data);
}
