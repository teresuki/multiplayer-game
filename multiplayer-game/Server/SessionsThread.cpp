#include "SessionsThread.h"
#include <tracy/Tracy.hpp>

void SessionsThread::OnClientConnectLockFree(ENetPeer client)
{
	ZoneScoped;
	m_IncomingClients.m_Queue.enqueue(std::move(client));
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

	_InitScheduler();
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

		// 1. Check for Scheduled Jobs
		scheduler.SchedulerLoop(deltaTime);
		
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
		deltaTime = endTimePoint - startTimePoint;
		FrameMark;
	}

}

void SessionsThread::_InitScheduler()
{
	std::function<void(SessionsThread*)> job = std::bind(&SessionsThread::_JobAcceptClientsFromQueue, this);
	scheduler.AddSchedule(this, job, std::chrono::microseconds(100));
}

void SessionsThread::_JobAcceptClientsFromQueue()
{
	ZoneNamedN(Zone_Job_Accept_Clients_From_Queue, "Job Accept Clients From Queue", true);
	ENetPeer peer;
	bool succeeded = false;
	while (m_IncomingClients.m_Queue.try_dequeue(peer))
	{
		ZoneNamedN(Zone_Dequeue_One_Client, "Dequeue One Client", true);
		auto sessionClientPair = std::make_pair(++SessionID, peer);
		m_ClientsMap.emplace(sessionClientPair);
	}
}

