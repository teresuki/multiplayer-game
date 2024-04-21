#include "SessionsThread.h"


void SessionsThread::StartThreadAsync()
{
	m_SessionsThread = std::jthread(&SessionsThread::_Loop, this);
}

void SessionsThread::OnClientConnect(ENetPeer client)
{
	std::lock_guard lckGuard(m_IncomingClients.m_QueueMtx);
	m_IncomingClients.m_Queue.push(client);
}

static uint64 SessionID = 0;
void SessionsThread::_Loop()
{
	// 1. Accept queued clients.
	{
		std::lock_guard lckGuard(m_IncomingClients.m_QueueMtx);
		while (m_IncomingClients.m_Queue.empty() == false)
		{
			auto sessionClientPair = std::make_pair(++SessionID, m_IncomingClients.m_Queue.front());
			m_ClientsMap.emplace(sessionClientPair);

			m_IncomingClients.m_Queue.pop();
		}
	}

	// 2. Loop over these clients and do something
	for (auto& [sessionID, client] : m_ClientsMap)
	{

	}
}

