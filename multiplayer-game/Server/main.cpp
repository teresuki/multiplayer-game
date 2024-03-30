//#define WIN32_LEAN_AND_MEAN			// If you want to include windows.h

/// WINDOWS SPECIFICS
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")


#include <Common/CommonHeader.h>
#include <Common/MyPacket.h>
#include <Common/Serializer.h>
#include <Common/PacketHandler.h>


#define now() std::chrono::steady_clock::now()

/// WINDOWS SPECIFICS
void WinAppStartup()
{
	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		fprintf(stderr, "WSAStartup failed.\n");
		exit(1);
	}

	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2)
	{
		fprintf(stderr, "Versiion 2.2 of Winsock is not available.\n");
		WSACleanup();
		exit(2);
	}
}

void ServerStart()
{

}


int main()
{
	for (int i = 0; i < 10; ++i)
	{
		MyPacket clientPacket{};
		clientPacket.myInt = 8457;
		clientPacket.myEnum = MyEnum::eOption2;
		clientPacket.myString = u8"Xin chào thế giới 세계를 향한 Use 不但 而且 for sentences contain both 不但 and 而且";
		clientPacket.myFloats = { -10.0f, 3.14f, 0.0001f };

		MySmallerStruct smaller{ {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20} };
		clientPacket.mySmallerStruct = std::move(smaller);

		clientPacket.myIntStringMap.emplace(std::make_pair(213, "Value String One"));
		clientPacket.myIntStringMap.emplace(std::make_pair(-10, "Minus Ten"));

		auto buffer = Serialize<MyPacket>(clientPacket);

		auto type = IdentifyPacket(buffer.get());
		if (PacketHandlerMap.count(type))
		{
			// invoke packet handler
			auto& packetHandler = PacketHandlerMap.at(type);
			packetHandler(buffer.get());
		}

	}

	return 0;
}