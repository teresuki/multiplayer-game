﻿//#define WIN32_LEAN_AND_MEAN			// If you want to include windows.h
/// WINDOWS SPECIFICS
//#include <winsock2.h>
//#include <ws2tcpip.h>
//#pragma comment(lib, "Ws2_32.lib")

#include <Common/CommonHeader.h>

#include "Example.h"
#include "NetworkThread.h"
#include "SessionsThread.h"

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

#include <tracy/Tracy.hpp>

int main()
{
	ZoneScoped;
	NetworkThread::instance().StartThreadAsync();
	SessionsThread::instance().StartThreadAsync();
	system("pause");

	return 0;
}