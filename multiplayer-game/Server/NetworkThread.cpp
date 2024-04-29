#include "NetworkThread.h"
#include "SessionsThread.h"
#include <tracy/Tracy.hpp>

#include "PacketHandler.h"

void NetworkThread::StartThreadAsync()
{
	ZoneScoped;
	m_NetworkThread  = std::jthread(&NetworkThread::_Setup, this);
}

int NetworkThread::_Setup()
{
	tracy::SetThreadName("Network Thread");
	ZoneScoped;
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

	_Start(address);

	return 0;
}

int NetworkThread::_Start(const ENetAddress& address)
{
	ZoneScoped;
	ENetHost* serverRawPtr{};

	serverRawPtr = enet_host_create(&address,	// the address to bind the server host to, in this case, localhost
		200,									// allow up to 200 clients
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
		_Loop(std::move(server));
	}
}


void TestDeserializeAndCallHandler(enet_uint8* recvPacket)
{
	char* _recvPacket = reinterpret_cast<char*>(recvPacket);
	PacketName type = IdentifyPacket(reinterpret_cast<const char*>(_recvPacket));
	if (PacketHandlerMap.count(type))
	{
		// invoke packet handler
		auto& packetHandler = PacketHandlerMap.at(type);
		packetHandler(_recvPacket);
	}
}

int NetworkThread::_Loop(std::unique_ptr<ENetHost> server)
{
	ZoneScoped;
	ENetEvent event{};
	size_t counter = 0;
	while (true) {
		while (enet_host_service(server.get(), &event, 0) > 0)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
			{
				ZoneNamedN(Zone_ENET_EVENT_TYPE_CONNECT, "ENET_EVENT_TYPE_CONNECT", true);
				//printf("A new client connected from %x:%u \n",
				//	event.peer->address.host,
				//	event.peer->address.port);

				event.peer->data = static_cast<void*>(const_cast<char*>("Client Information"));
				//SessionsThread::instance().OnClientConnect(*event.peer);
				SessionsThread::instance().OnClientConnectLockFree(*event.peer);
				break;
			}
			case ENET_EVENT_TYPE_RECEIVE:
			{
				ZoneNamedN(Zone_ENET_EVENT_TYPE_RECEIVE, "ENET_EVENT_TYPE_RECEIVE", true);
				//printf("A packet of length %u containing %s was received from %s on channel %u.\n",
				//	event.packet->dataLength,
				//	event.packet->data,
				//	event.peer->data,
				//	event.channelID);
				//std::cout << ++counter << '\n';
				/* Clean up the packet now that we're done using it. */
				TestDeserializeAndCallHandler(event.packet->data);



				enet_packet_destroy(event.packet);

				break;
			}
			case ENET_EVENT_TYPE_DISCONNECT:
			{
				printf("%s disconnected. \n", event.peer->data);

				/* Reset the peer's client information */
				event.peer->data = nullptr;
				break;
			}
			}

		}
	}
}
