#pragma once

#include "DEngine/Core.h"

#include <cstdint>
#include <functional>

namespace DEngine
{
	struct JobRange
	{
		uint32_t Start = 0;
		uint32_t End = 0;
	};

	enum class JobPriority : uint8_t
	{
		High = 0,
		Medium,
		Low
	};

	using JobFunction = std::function<void(const JobRange& range, uint32_t threadIndex)>;
	using PinnedJobFunction = std::function<void()>;

	class PinnedJob;

	// Range job. Keep it alive until IsComplete() is true. DependsOn before Schedule.
	class D_API Job
	{
	public:
		Job();
		explicit Job(JobFunction function);
		Job(uint32_t setSize, JobFunction function, uint32_t minRange = 1);
		~Job();

		Job(Job&& other) noexcept;
		Job& operator=(Job&& other) noexcept;
		Job(const Job&) = delete;
		Job& operator=(const Job&) = delete;

		void SetFunction(JobFunction function);
		void SetSize(uint32_t setSize);
		void SetMinRange(uint32_t minRange);
		void SetPriority(JobPriority priority);

		void DependsOn(const Job& dependency);
		void DependsOn(const PinnedJob& dependency);
		void ClearDependencies();

		bool IsComplete() const;

	private:
		friend class PinnedJob;
		friend class JobSystem;

		struct Data;
		Scope<Data> m_Data;
	};

	// Runs on one scheduler thread. Thread 0 is the thread that called JobSystem::Init.
	class D_API PinnedJob
	{
	public:
		PinnedJob();
		explicit PinnedJob(PinnedJobFunction function);
		PinnedJob(uint32_t threadIndex, PinnedJobFunction function);
		~PinnedJob();

		PinnedJob(PinnedJob&& other) noexcept;
		PinnedJob& operator=(PinnedJob&& other) noexcept;
		PinnedJob(const PinnedJob&) = delete;
		PinnedJob& operator=(const PinnedJob&) = delete;

		void SetFunction(PinnedJobFunction function);
		void SetThread(uint32_t threadIndex);
		void SetPriority(JobPriority priority);

		void DependsOn(const Job& dependency);
		void DependsOn(const PinnedJob& dependency);
		void ClearDependencies();

		bool IsComplete() const;

	private:
		friend class Job;
		friend class JobSystem;

		struct Data;
		Scope<Data> m_Data;
	};

	class D_API JobSystem
	{
	public:
		static constexpr uint32_t InvalidThread = 0xFFFFFFFFu;

		struct Config
		{
			// Total threads, including the caller. 0 uses the hardware count.
			uint32_t ThreadCount = 0;
			// Extra threads that may call Schedule, Wait, or ParallelFor.
			uint32_t ExternalThreadCount = 0;
		};

		static void Init(const Config& config = {});
		static void Shutdown();
		static bool IsInitialized();

		static uint32_t HardwareThreadCount();
		static uint32_t ThreadCount();
		static uint32_t ThreadIndex();

		static bool IsShutdownRequested();
		static bool IsWaitingForAll();

		static bool RegisterThisThread();
		static bool RegisterThisThread(uint32_t threadIndex);
		static void UnregisterThisThread();

		static void Schedule(Job& job);
		static void Schedule(PinnedJob& job);

		static void Wait(const Job& job, JobPriority lowestPriority = JobPriority::Low);
		static void Wait(const PinnedJob& job, JobPriority lowestPriority = JobPriority::Low);
		static void WaitForAll();
		static void RunPinnedTasks();
		static void WaitForPinnedTasks();

		static void ParallelFor(uint32_t count, const JobFunction& function, uint32_t minRange = 1, JobPriority priority = JobPriority::High);
		static void Execute(const std::function<void()>& function);
	};
}
