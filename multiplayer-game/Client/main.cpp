#define ENET_IMPLEMENTATION
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <enet/enet.h>

#include <Common/CommonHeader.h>
int ClientLoop(std::unique_ptr<ENetHost> client, std::unique_ptr<ENetPeer> serverPeer)
{
	ENetEvent event{};

	/* CLIENT GAME LOOP START */
	while (true) {
		//std::cout << "[CLIENT] [TRACE] enet_host_service looping...\n";

		while (enet_host_service(client.get(), &event, 50) > 0) {};
		/* Create a reliable packet of size 7 containing "packet\0" */
		ENetPacket* packet = enet_packet_create("Teresuki Kitsune", strlen("Teresuki Kitsune") + 1, ENET_PACKET_FLAG_RELIABLE);

		/* Send the packet to the peer over channel id 0. */
		/* One could also broadcast the packet by         */
		/* enet_host_broadcast (host, 0, packet);         */
		enet_peer_send(serverPeer.get(), 0, packet);
}

	/* CLIENT GAME LOOP END */

}

int ClientStart(std::unique_ptr<ENetHost> client)
{
	ENetAddress serverAddress{};
	ENetPeer* serverPeerRawPtr{};

	/* Setup the server information */
	enet_address_set_host(&serverAddress, "127.0.0.1");
	serverAddress.port = 7777;

	/* Initiate the connection, allocatign the two channel 0 and 1 */
	serverPeerRawPtr = enet_host_connect(client.get(), &serverAddress, 2, 0);

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
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	if (enet_initialize() != 0) {
		fprintf(stderr, "An error occurred while initializing ENet.\n");
		return -1;
	}

	ENetHost* clientRawPtr{};

	clientRawPtr = enet_host_create(nullptr,		// Create a client host
		1,											// only allow 1 outgoing connection
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
}

int main()
{
	SetupClient();
}