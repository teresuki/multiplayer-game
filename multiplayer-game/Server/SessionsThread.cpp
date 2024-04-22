#include "SessionsThread.h"
#include <tracy/Tracy.hpp>

void SessionsThread::OnClientConnect(ENetPeer client)
{
	ZoneScoped;
	std::lock_guard lckGuard(m_IncomingClients.m_QueueMtx);
	m_IncomingClients.m_Queue.push(client);
	m_IncomingClients.m_CondVarQueueMtx.notify_all();
}

void SessionsThread::StartThreadAsync()
{
	ZoneScoped;
	m_SessionsThread = std::jthread(&SessionsThread::_Init, this);
}

void SessionsThread::_Init()
{
	tracy::SetThreadName("Sessions Thread");
	ZoneScoped;
	m_IncomingClients.m_AcceptInterval = std::chrono::high_resolution_clock::duration(300'000'000);	// Accept new clients every 300 milliseconds.
	m_IncomingClients.m_AcceptElapsedTime = std::chrono::high_resolution_clock::duration(0);

	_Loop();
}

static uint64 SessionID = 0;
void SessionsThread::_Loop()
{
	ZoneScoped;
	std::chrono::high_resolution_clock::duration deltaTime = std::chrono::high_resolution_clock::duration(0);
	std::chrono::high_resolution_clock::time_point startTimePoint{};
	std::chrono::high_resolution_clock::time_point endTimePoint{};
	while (true)
	{
		startTimePoint = now();
		// 1. Accept queued clients.
		if(m_IncomingClients.m_AcceptElapsedTime > m_IncomingClients.m_AcceptInterval)
		{
			ZoneNamedN(Zone_Accept_Queued_Clients, "Accept Queued Clients Mutex Lock", true);
			std::unique_lock lock(m_IncomingClients.m_QueueMtx);
			auto isDataReady = m_IncomingClients.m_CondVarQueueMtx.wait_for(lock, std::chrono::nanoseconds(500), [this]() {return !m_IncomingClients.m_Queue.empty(); });

			if (isDataReady)
			{
				ZoneNamedN(Zone_Accept_Queued_Clients, "Accept Queued Clients", true);
				while (m_IncomingClients.m_Queue.empty() == false)
				{
					auto sessionClientPair = std::make_pair(++SessionID, m_IncomingClients.m_Queue.front());
					m_ClientsMap.emplace(sessionClientPair);

					m_IncomingClients.m_Queue.pop();
				}
			}

			m_IncomingClients.m_AcceptElapsedTime = std::chrono::high_resolution_clock::duration(0);
		}

		// 2. Loop over these clients and do something
		{
			for (auto& [sessionID, client] : m_ClientsMap)
			{
				//ZoneNamedN(Zone_Sessions_Clients_Loop, "Sessions Clients Loop", true);
			}
		}
		// X. Fake task to slow down the loop
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		// 3. Update delta time
		endTimePoint = now();
		m_IncomingClients.m_AcceptElapsedTime += (endTimePoint - startTimePoint);
		FrameMark;
	}



}

