#pragma once

#include <Common/CommonHeader.h>
#include <Common/Singleton.h>
#include <Common/Schedule.h>
#include <Common/LockFreeSPCSQueue.h>
#include <enet/enet.h>
#include <tracy/Tracy.hpp>

class SessionsThread final : public Singleton<SessionsThread>
{
public:
	void StartThreadAsync();

public:
	void OnClientConnectLockFree(ENetPeer client);
private:
	void _Init();
	void _Loop();

	void _InitScheduler();
	void _JobAcceptClientsFromQueue();
private:
	struct IncomingClients
	{
		moodycamel::ReaderWriterQueue<ENetPeer> m_Queue;
	} m_IncomingClients;

	Scheduler<SessionsThread> scheduler;
	std::map<uint64, ENetPeer> m_ClientsMap;
private:
	std::jthread m_SessionsThread;
};
