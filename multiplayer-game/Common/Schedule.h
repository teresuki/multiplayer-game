#pragma once
#include "CommonHeader.h"

template <typename HostType>
struct Schedule
{
	HostType* const										m_Executor{};
	const std::function<void(HostType*)>				m_Job{};				// The job to run when time interval is reached.
	const std::chrono::high_resolution_clock::duration  m_Interval{};			// Interval in which the job will be execute (Example: For each 2 seconds)
	std::chrono::high_resolution_clock::duration		m_Elapsed{};			// Time elapsed since the previous job was executed.

	// Constructor
	Schedule(HostType* const executor, const std::function<void(HostType*)> job, const std::chrono::high_resolution_clock::duration interval)
		:m_Executor(executor), m_Job(job), m_Interval(interval), m_Elapsed(std::chrono::high_resolution_clock::duration(0))
	{

	}

	inline void ExecuteJob()
	{
		m_Job(m_Executor);
	}

	inline void ResetTimer()
	{
		m_Elapsed = std::chrono::high_resolution_clock::duration(0);
	}
};


template <typename HostType>
class Scheduler final
{
public:
	Scheduler() = default;
	~Scheduler() = default;

	void AddSchedule(HostType* const executor, const std::function<void(HostType*)> job, const std::chrono::high_resolution_clock::duration interval)
	{
		auto schedule = Schedule<HostType>(executor, job, interval);
		m_Schedules.push_back(schedule);
	}

	void SchedulerLoop(std::chrono::high_resolution_clock::duration elapsed)
	{
		for (auto& schedule : m_Schedules)
		{
			schedule.m_Elapsed += elapsed;

			// Execute the job if interval time has been reached.
			if (schedule.m_Elapsed >= schedule.m_Interval)
			{
				schedule.ExecuteJob();
				schedule.ResetTimer();
			}
		}
	}

private:
	std::vector<Schedule<HostType>> m_Schedules;
};