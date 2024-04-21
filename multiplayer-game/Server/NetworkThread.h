#pragma once

#include <Common/CommonHeader.h>
#include <Common/Singleton.h>
#include <enet/enet.h>

class NetworkThread final : public Singleton<NetworkThread>
{
public:
	void StartThreadAsync();
private:
	int _Setup();
	int _Start(const ENetAddress& address);
	int _Loop(std::unique_ptr<ENetHost> server);
private:
	std::jthread m_NetworkThread;
};
