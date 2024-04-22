#include <enet/enet.h>
#include <Common/CommonHeader.h>
#include <Common/MyPacket.h>
#include <Common/Serializer.h>
#include <tracy/Tracy.hpp>


MyPacket TestCreatePacket()
{
	MyPacket clientPacket{};
	clientPacket.myInt = 8457;
	clientPacket.myEnum = MyEnum::eOption2;
	clientPacket.myString = u8"Xin chào thế giới 세계를 향한 Use 不但 而且 for sentences contain both 不但 and 而且";
	clientPacket.myFloats = { -10.0f, 3.14f, 0.0001f, -202.202f };

	MySmallerStruct smaller{};
	for (uint64 i = 0; i < 20; ++i)
	{
		smaller.multipleInts.push_back(i);
	}
	clientPacket.mySmallerStruct = std::move(smaller);

	clientPacket.myIntStringMap.emplace(std::make_pair(213, "Value String One"));
	clientPacket.myIntStringMap.emplace(std::make_pair(-10, "Minus Ten"));

	return clientPacket;
}

int ClientLoop(std::unique_ptr<ENetHost> client, std::unique_ptr<ENetPeer> serverPeer)
{
	ZoneScoped;
	ENetEvent event{};

	/* CLIENT GAME LOOP START */
	while (true) {
		while (enet_host_service(client.get(), &event, 50) > 0) {};

		/* Create a reliable packet */
		{
			for (int i = 0; i < 200; ++i)
			{
				MyPacket sendingPacket = TestCreatePacket();
				ZoneNamedN(Zone_Serialize_Packet_And_Send, "Serialize Packet and Send", true);
				auto [buffer, bufferSize] = Serialize(sendingPacket);

				ENetPacket* packet = enet_packet_create(buffer.get(), bufferSize, ENET_PACKET_FLAG_RELIABLE);

				/* Send the packet to the peer over channel id 0. */
				/* One could also broadcast the packet by         */
				/* enet_host_broadcast (host, 0, packet);         */
				/* enet_host_broadcast seems useful for server */
				enet_peer_send(serverPeer.get(), 0, packet);
			}
		}

}

	/* CLIENT GAME LOOP END */

}

int ClientStart(std::unique_ptr<ENetHost> client)
{
	ZoneScoped;
	ENetAddress serverAddress{};
	ENetPeer* serverPeerRawPtr{};

	/* Setup the server information */
	enet_address_set_host(&serverAddress, "127.0.0.1");
	serverAddress.port = 7777;

	/* Initiate the connection, allocatign the two channel 0 and 1 */
	for (int i = 0; i < 200; ++i)
	{
		serverPeerRawPtr = enet_host_connect(client.get(), &serverAddress, 2, 0);
	}

	if (serverPeerRawPtr == nullptr)
	{
		std::cerr << "No available peers for initiating an ENet connection.\n";
		return -1;
	}

	ENetEvent event{};

	/* Wait up to 5 seconds for the connection attempt to succeed. */
	if (enet_host_service(client.get(), &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
	{
		std::cout << "Connection to localhost:7777 succeeded.\n";
		event.peer->data = static_cast<void*>(const_cast<char*>("Server information"));
	}
	else
	{
		/* Either the 5 seconds are up or a disconnect event was */
		/* received. Reset the peer in the event the 5 seconds   */
		/* had run out without any significant event.            */
		enet_peer_reset(serverPeerRawPtr);

		std::cout << "Connection to localhost:7777 failed.\n";
		return -1;
	}

	std::unique_ptr<ENetPeer> serverPeer(serverPeerRawPtr);
	ClientLoop(std::move(client), std::move(serverPeer));
	return 0;
}

int SetupClient()
{
	ZoneScoped;
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	if (enet_initialize() != 0) {
		fprintf(stderr, "An error occurred while initializing ENet.\n");
		return -1;
	}

	ENetHost* clientRawPtr{};

	clientRawPtr = enet_host_create(nullptr,		// Create a client host
		200,											// only allow 1 outgoing connection
		2,											// allow up to 2 channels to be used, 0 and 1
		0,											// assume any amount of incoming bandwidth
		0);											// assume any amount of outgoing bandwidth

	if (clientRawPtr == nullptr)
	{
		std::cerr << "An error occurred while trying to create an ENet client host.\n";
		return -1;
	}

	std::unique_ptr<ENetHost> client(clientRawPtr);
	ClientStart(std::move(client));

	return 0;
}

int main()
{
	ZoneScoped;
	SetupClient();
}