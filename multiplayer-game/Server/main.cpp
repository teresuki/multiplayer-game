//#define WIN32_LEAN_AND_MEAN			// If you want to include windows.h
/// WINDOWS SPECIFICS
//#include <winsock2.h>
//#include <ws2tcpip.h>
//#pragma comment(lib, "Ws2_32.lib")

#include "pch.h"

#include "Example.h"
#define ENET_IMPLEMENTATION
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "enet.h"

/// WINDOWS SPECIFICS
//void WinAppStartup()
//{
//	WSADATA wsaData;
//
//	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
//		fprintf(stderr, "WSAStartup failed.\n");
//		exit(1);
//	}
//
//	if (LOBYTE(wsaData.wVersion) != 2 ||
//		HIBYTE(wsaData.wVersion) != 2)
//	{
//		fprintf(stderr, "Version 2.2 of Winsock is not available.\n");
//		WSACleanup();
//		exit(2);
//	}
//}

int ServerLoop(std::unique_ptr<ENetHost> server)
{
	ENetEvent event{};
	size_t counter = 0;
	while (true){
	std::cout << "[SERVER] [TRACE] enet_host_service looping...\n";
	while (enet_host_service(server.get(), &event, 1000) > 0)
	{
		switch (event.type)
		{
		case ENET_EVENT_TYPE_CONNECT:
		{
			printf("A new client connected from %x:%u \n",
				event.peer->address.host,
				event.peer->address.port);

			event.peer->data = static_cast<void*>(const_cast<char*>("Client Information"));
			break;
		}
		case ENET_EVENT_TYPE_RECEIVE:
		{
			//printf("A packet of length %u containing %s was received from %s on channel %u.\n",
			//	event.packet->dataLength,
			//	event.packet->data,
			//	event.peer->data,
			//	event.channelID);
			std::cout << ++counter << '\n';
			/* Clean up the packet now that we're done using it. */
			enet_packet_destroy(event.packet);

			break;
		}
		}

	}
}

	/* GAME_LOOP END */
	enet_host_destroy(server.get());
	enet_deinitialize();

	return 0;
}

int ServerStart(const ENetAddress& address)
{
	ENetHost* serverRawPtr{};

	serverRawPtr = enet_host_create(&address,	// the address to bind the server host to, in this case, localhost
		32,										// allow up to 32 clients
		2,										// allow up to two channels can be used: 0 and 1
		0,										// no assumption for incoming bandwidth
		0);										// no assumpition for outgoing bandwidth

	if (serverRawPtr == nullptr)
	{
		std::cerr << "An error occurred while trying to create an ENet server host!\n";
		return -1;
	}
	
	std::unique_ptr<ENetHost> server(serverRawPtr);

	while (true)
	{
		ServerLoop(std::move(server));
	}
}

int SetupServer()
{
	bool ENetIsInitialized = enet_initialize();
	if (ENetIsInitialized != 0)
	{
		std::cerr << "Enet failed to initialized!\n";
		return -1;
	}

	ENetAddress address{};

	/**
	*  Bind the server to the default localhost.
	* A specific host address can be specified by
	* enet_address_set_host (&address, "x.x.x.x");
	*/

	address.host = ENET_HOST_ANY;
	address.port = 7777;			// Bind our server to port 7777

	ServerStart(address);
}

int main()
{
	//Example::AllDemo();

	SetupServer();

	system("pause");

	return 0;
}