#include "dpch.h"
#include "JobSystem.h"

#include "TaskScheduler.h"

#include <utility>

namespace DEngine
{
	namespace
	{
		std::unique_ptr<enki::TaskScheduler> g_Scheduler;

		enki::TaskPriority ToEnkiPriority(JobPriority priority)
		{
			switch (priority)
			{
			case JobPriority::Medium: return enki::TASK_PRIORITY_MED;
			case JobPriority::Low: return enki::TASK_PRIORITY_LOW;
			default: return enki::TASK_PRIORITY_HIGH;
			}
		}

		enki::TaskScheduler* Scheduler()
		{
			D_CORE_ASSERT(g_Scheduler, "JobSystem::Init was not called");
			return g_Scheduler.get();
		}

		void LinkDependency(std::vector<Scope<enki::Dependency>>& dependencies, const enki::ICompletable* dependency, enki::ICompletable* task)
		{
			D_CORE_ASSERT(dependency && task, "Job dependency is empty");
			D_CORE_ASSERT(dependency != task, "A job cannot depend on itself");
			dependencies.push_back(CreateScope<enki::Dependency>(dependency, task));
		}
	}

	struct Job::Data
	{
		enki::TaskSet Task;
		std::vector<Scope<enki::Dependency>> Dependencies;
	};

	struct PinnedJob::Data
	{
		enki::LambdaPinnedTask Task;
		std::vector<Scope<enki::Dependency>> Dependencies;
	};

	Job::Job()
		: m_Data(CreateScope<Data>())
	{
	}

	Job::Job(JobFunction function)
		: Job()
	{
		SetFunction(std::move(function));
	}

	Job::Job(uint32_t setSize, JobFunction function, uint32_t minRange)
		: Job(std::move(function))
	{
		SetSize(setSize);
		SetMinRange(minRange);
	}

	Job::~Job()
	{
		D_CORE_ASSERT(IsComplete(), "Job destroyed while running");
	}

	Job::Job(Job&& other) noexcept
		: m_Data(std::move(other.m_Data))
	{
	}

	Job& Job::operator=(Job&& other) noexcept
	{
		if (this != &other)
		{
			D_CORE_ASSERT(IsComplete(), "Cannot replace a running job");
			m_Data = std::move(other.m_Data);
		}
		return *this;
	}

	void Job::SetFunction(JobFunction function)
	{
		D_CORE_ASSERT(IsComplete(), "Cannot edit a running job");
		D_CORE_ASSERT(m_Data, "Job is empty");
		m_Data->Task.m_Function = [function = std::move(function)](enki::TaskSetPartition range, uint32_t threadIndex)
		{
			if (function)
				function(JobRange{ range.start, range.end }, threadIndex);
		};
	}

	void Job::SetSize(uint32_t setSize)
	{
		D_CORE_ASSERT(IsComplete(), "Cannot edit a running job");
		D_CORE_ASSERT(m_Data, "Job is empty");
		m_Data->Task.m_SetSize = setSize;
	}

	void Job::SetMinRange(uint32_t minRange)
	{
		D_CORE_ASSERT(IsComplete(), "Cannot edit a running job");
		D_CORE_ASSERT(m_Data, "Job is empty");
		m_Data->Task.m_MinRange = minRange == 0 ? 1 : minRange;
	}

	void Job::SetPriority(JobPriority priority)
	{
		D_CORE_ASSERT(IsComplete(), "Cannot edit a running job");
		D_CORE_ASSERT(m_Data, "Job is empty");
		m_Data->Task.m_Priority = ToEnkiPriority(priority);
	}

	void Job::DependsOn(const Job& dependency)
	{
		D_CORE_ASSERT(IsComplete(), "Cannot add a dependency to a running job");
		D_CORE_ASSERT(m_Data && dependency.m_Data, "Job is empty");
		LinkDependency(m_Data->Dependencies, &dependency.m_Data->Task, &m_Data->Task);
	}

	void Job::DependsOn(const PinnedJob& dependency)
	{
		D_CORE_ASSERT(IsComplete(), "Cannot add a dependency to a running job");
		D_CORE_ASSERT(m_Data && dependency.m_Data, "Job is empty");
		LinkDependency(m_Data->Dependencies, &dependency.m_Data->Task, &m_Data->Task);
	}

	void Job::ClearDependencies()
	{
		D_CORE_ASSERT(IsComplete(), "Cannot edit a running job");
		D_CORE_ASSERT(m_Data, "Job is empty");
		for (auto& dependency : m_Data->Dependencies)
			dependency->ClearDependency();
		m_Data->Dependencies.clear();
	}

	bool Job::IsComplete() const
	{
		if (!m_Data)
			return true;
		return m_Data->Task.GetIsComplete();
	}

	PinnedJob::PinnedJob()
		: m_Data(CreateScope<Data>())
	{
	}

	PinnedJob::PinnedJob(PinnedJobFunction function)
		: PinnedJob()
	{
		SetFunction(std::move(function));
	}

	PinnedJob::PinnedJob(uint32_t threadIndex, PinnedJobFunction function)
		: PinnedJob(std::move(function))
	{
		SetThread(threadIndex);
	}

	PinnedJob::~PinnedJob()
	{
		D_CORE_ASSERT(IsComplete(), "Pinned job destroyed while running");
	}

	PinnedJob::PinnedJob(PinnedJob&& other) noexcept
		: m_Data(std::move(other.m_Data))
	{
	}

	PinnedJob& PinnedJob::operator=(PinnedJob&& other) noexcept
	{
		if (this != &other)
		{
			D_CORE_ASSERT(IsComplete(), "Cannot replace a running pinned job");
			m_Data = std::move(other.m_Data);
		}
		return *this;
	}

	void PinnedJob::SetFunction(PinnedJobFunction function)
	{
		D_CORE_ASSERT(IsComplete(), "Cannot edit a running pinned job");
		D_CORE_ASSERT(m_Data, "Pinned job is empty");
		m_Data->Task.m_Function = std::move(function);
	}

	void PinnedJob::SetThread(uint32_t threadIndex)
	{
		D_CORE_ASSERT(IsComplete(), "Cannot edit a running pinned job");
		D_CORE_ASSERT(m_Data, "Pinned job is empty");
		m_Data->Task.threadNum = threadIndex;
	}

	void PinnedJob::SetPriority(JobPriority priority)
	{
		D_CORE_ASSERT(IsComplete(), "Cannot edit a running pinned job");
		D_CORE_ASSERT(m_Data, "Pinned job is empty");
		m_Data->Task.m_Priority = ToEnkiPriority(priority);
	}

	void PinnedJob::DependsOn(const Job& dependency)
	{
		D_CORE_ASSERT(IsComplete(), "Cannot add a dependency to a running pinned job");
		D_CORE_ASSERT(m_Data && dependency.m_Data, "Pinned job is empty");
		LinkDependency(m_Data->Dependencies, &dependency.m_Data->Task, &m_Data->Task);
	}

	void PinnedJob::DependsOn(const PinnedJob& dependency)
	{
		D_CORE_ASSERT(IsComplete(), "Cannot add a dependency to a running pinned job");
		D_CORE_ASSERT(m_Data && dependency.m_Data, "Pinned job is empty");
		LinkDependency(m_Data->Dependencies, &dependency.m_Data->Task, &m_Data->Task);
	}

	void PinnedJob::ClearDependencies()
	{
		D_CORE_ASSERT(IsComplete(), "Cannot edit a running pinned job");
		D_CORE_ASSERT(m_Data, "Pinned job is empty");
		for (auto& dependency : m_Data->Dependencies)
			dependency->ClearDependency();
		m_Data->Dependencies.clear();
	}

	bool PinnedJob::IsComplete() const
	{
		if (!m_Data)
			return true;
		return m_Data->Task.GetIsComplete();
	}

	void JobSystem::Init(const Config& config)
	{
		D_CORE_ASSERT(!g_Scheduler, "JobSystem is already initialized");
		if (g_Scheduler)
			return;

		if (config.ThreadCount == 1)
		{
			D_CORE_ASSERT(false, "JobSystem needs at least two threads");
			return;
		}

		g_Scheduler = CreateScope<enki::TaskScheduler>();
		enki::TaskSchedulerConfig enkiConfig = g_Scheduler->GetConfig();
		if (config.ThreadCount > 1)
			enkiConfig.numTaskThreadsToCreate = config.ThreadCount - 1;
		enkiConfig.numExternalTaskThreads = config.ExternalThreadCount;

		g_Scheduler->Initialize(enkiConfig);

		D_CORE_INFO("Init Job system");
	}

	void JobSystem::Shutdown()
	{
		if (!g_Scheduler)
			return;

		g_Scheduler->WaitforAllAndShutdown();
		g_Scheduler.reset();

		D_CORE_INFO("Shutdown Job system");
	}

	bool JobSystem::IsInitialized()
	{
		return g_Scheduler != nullptr;
	}

	uint32_t JobSystem::HardwareThreadCount()
	{
		return enki::GetNumHardwareThreads();
	}

	uint32_t JobSystem::ThreadCount()
	{
		enki::TaskScheduler* scheduler = Scheduler();
		if (!scheduler)
			return 0;
		return scheduler->GetNumTaskThreads();
	}

	uint32_t JobSystem::ThreadIndex()
	{
		enki::TaskScheduler* scheduler = Scheduler();
		if (!scheduler)
			return InvalidThread;
		return scheduler->GetThreadNum();
	}

	bool JobSystem::IsShutdownRequested()
	{
		enki::TaskScheduler* scheduler = Scheduler();
		if (!scheduler)
			return false;
		return scheduler->GetIsShutdownRequested();
	}

	bool JobSystem::IsWaitingForAll()
	{
		enki::TaskScheduler* scheduler = Scheduler();
		if (!scheduler)
			return false;
		return scheduler->GetIsWaitforAllCalled();
	}

	bool JobSystem::RegisterThisThread()
	{
		enki::TaskScheduler* scheduler = Scheduler();
		if (!scheduler)
			return false;
		return scheduler->RegisterExternalTaskThread();
	}

	bool JobSystem::RegisterThisThread(uint32_t threadIndex)
	{
		enki::TaskScheduler* scheduler = Scheduler();
		if (!scheduler)
			return false;
		return scheduler->RegisterExternalTaskThread(threadIndex);
	}

	void JobSystem::UnregisterThisThread()
	{
		enki::TaskScheduler* scheduler = Scheduler();
		if (!scheduler)
			return;
		scheduler->DeRegisterExternalTaskThread();
	}

	void JobSystem::Schedule(Job& job)
	{
		enki::TaskScheduler* scheduler = Scheduler();
		if (!scheduler)
			return;

		D_CORE_ASSERT(job.m_Data, "Job is empty");
		D_CORE_ASSERT(job.IsComplete(), "Job is still running");
		D_CORE_ASSERT(job.m_Data->Task.m_SetSize > 0, "Job size is 0");
		D_CORE_ASSERT(job.m_Data->Task.m_Function, "Job has no function");
		scheduler->AddTaskSetToPipe(&job.m_Data->Task);
	}

	void JobSystem::Schedule(PinnedJob& job)
	{
		enki::TaskScheduler* scheduler = Scheduler();
		if (!scheduler)
			return;

		D_CORE_ASSERT(job.m_Data, "Pinned job is empty");
		D_CORE_ASSERT(job.IsComplete(), "Pinned job is still running");
		D_CORE_ASSERT(job.m_Data->Task.m_Function, "Pinned job has no function");
		scheduler->AddPinnedTask(&job.m_Data->Task);
	}

	void JobSystem::Wait(const Job& job, JobPriority lowestPriority)
	{
		enki::TaskScheduler* scheduler = Scheduler();
		if (!scheduler)
			return;

		D_CORE_ASSERT(job.m_Data, "Job is empty");
		scheduler->WaitforTask(&job.m_Data->Task, ToEnkiPriority(lowestPriority));
	}

	void JobSystem::Wait(const PinnedJob& job, JobPriority lowestPriority)
	{
		enki::TaskScheduler* scheduler = Scheduler();
		if (!scheduler)
			return;

		D_CORE_ASSERT(job.m_Data, "Pinned job is empty");
		scheduler->WaitforTask(&job.m_Data->Task, ToEnkiPriority(lowestPriority));
	}

	void JobSystem::WaitForAll()
	{
		enki::TaskScheduler* scheduler = Scheduler();
		if (!scheduler)
			return;
		scheduler->WaitforAll();
	}

	void JobSystem::RunPinnedTasks()
	{
		enki::TaskScheduler* scheduler = Scheduler();
		if (!scheduler)
			return;
		scheduler->RunPinnedTasks();
	}

	void JobSystem::WaitForPinnedTasks()
	{
		enki::TaskScheduler* scheduler = Scheduler();
		if (!scheduler)
			return;
		scheduler->WaitForNewPinnedTasks();
	}

	void JobSystem::ParallelFor(uint32_t count, const JobFunction& function, uint32_t minRange, JobPriority priority)
	{
		enki::TaskScheduler* scheduler = Scheduler();
		if (!scheduler || count == 0 || !function)
			return;

		enki::TaskSet task(count, [function](enki::TaskSetPartition range, uint32_t threadIndex)
		{
			function(JobRange{ range.start, range.end }, threadIndex);
		});
		task.m_MinRange = minRange == 0 ? 1 : minRange;
		task.m_Priority = ToEnkiPriority(priority);
		scheduler->AddTaskSetToPipe(&task);
		scheduler->WaitforTask(&task);
	}

	void JobSystem::Execute(const std::function<void()>& function)
	{
		if (!function)
			return;

		ParallelFor(1, [function](const JobRange&, uint32_t)
		{
			function();
		});
	}
}
