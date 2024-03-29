//#define WIN32_LEAN_AND_MEAN			// If you want to include windows.h

/// WINDOWS SPECIFICS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstring>
#pragma comment(lib, "Ws2_32.lib")


#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include <memory>
#include <chrono>
#include <functional>



#include "CommonHeader.h"
#include "ExampleNetStruct.h"
#include "Serializer.h"
#include "NetPacketHandler.h"



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
		MyStruct clientStruct{ 8457, MyEnum::eOption3, u8"Xin chào thế giới 세계를 향한 Use 不但 而且 for sentences contain both 不但 and 而且", {-10.0f, 3.14f, 0.0001f} };
		MySmallerStruct smaller{ {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20} };
		clientStruct.mySmallerStruct = smaller;

		clientStruct.myIntStringMap.emplace(std::make_pair(213, "Value String One"));
		clientStruct.myIntStringMap.emplace(std::make_pair(-10, "Minus Ten"));

		auto buffer = Serialize<MyStruct>(clientStruct);

		auto type = IdentifyPacket(buffer.get());
		if (NetPacketHandlerMap.count(type))
		{
			// invoke packet handler
			auto& packetHandler = NetPacketHandlerMap.at(type);
			packetHandler(buffer.get());
		}

	}

	return 0;
}