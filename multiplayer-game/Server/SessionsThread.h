#pragma once

#include <Common/CommonHeader.h>
#include <Common/Singleton.h>
#include <enet/enet.h>

class SessionsThread final : public Singleton<SessionsThread>
{
public:
	void StartThreadAsync();

public:
	void OnClientConnect(ENetPeer client);
private:
	void _Loop();
private:
	struct IncomingClients
	{
		std::queue<ENetPeer> m_Queue;
		std::mutex m_QueueMtx;
	} m_IncomingClients;

	std::map<uint64, ENetPeer> m_ClientsMap;
private:
	std::jthread m_SessionsThread;
};
