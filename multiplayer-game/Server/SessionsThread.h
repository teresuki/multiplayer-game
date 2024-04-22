#pragma once

#include <Common/CommonHeader.h>
#include <Common/Singleton.h>
#include <enet/enet.h>
#include <tracy/Tracy.hpp>

class SessionsThread final : public Singleton<SessionsThread>
{
public:
	void StartThreadAsync();

public:
	void OnClientConnect(ENetPeer client);
private:
	void _Init();
	void _Loop();
private:
	struct IncomingClients
	{
		std::queue<ENetPeer> m_Queue;
		TracyLockable(std::mutex, m_QueueMtx);
		std::condition_variable_any m_CondVarQueueMtx{};

		std::chrono::high_resolution_clock::duration m_AcceptInterval;
		std::chrono::high_resolution_clock::duration m_AcceptElapsedTime;
	} m_IncomingClients;

	std::map<uint64, ENetPeer> m_ClientsMap;
private:
	std::jthread m_SessionsThread;
};
