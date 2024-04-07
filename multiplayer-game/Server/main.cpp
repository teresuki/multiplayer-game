//#define WIN32_LEAN_AND_MEAN			// If you want to include windows.h

/// WINDOWS SPECIFICS
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")


#include <Common/CommonHeader.h>
#include <Common/MyPacket.h>
#include <Common/Serializer.h>
#include "PacketHandler.h"

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
		clientPacket.myFloats = { -10.0f, 3.14f, 0.0001f, -202.202f };

		MySmallerStruct smaller{};
		for (uint64 i = 0; i < 20; ++i)
		{
			smaller.multipleInts.push_back(i);
		}
		clientPacket.mySmallerStruct = std::move(smaller);

		clientPacket.myIntStringMap.emplace(std::make_pair(213, "Value String One"));
		clientPacket.myIntStringMap.emplace(std::make_pair(-10, "Minus Ten"));

		auto start = now();
		auto buffer = Serialize<MyPacket>(clientPacket);
		auto end = now();
		std::cout << "Serialize time: " << (end - start).count() << "ns.\n";

		auto type = IdentifyPacket(buffer.get());
		if (PacketHandlerMap.count(type))
		{
			// invoke packet handler
			auto& packetHandler = PacketHandlerMap.at(type);
			packetHandler(buffer.get());
		}
	}

	system("pause");
	return 0;
}