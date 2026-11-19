#include "dpch.h"
#include "CommandPoolSystem.h"

#include "DEngine/Jobs/JobSystem.h"

namespace DEngine
{
	std::vector<Ref<CommandPool>> CommandPoolSystem::m_Pools;
	Ref<CommandPool> CommandPoolSystem::m_Empty;

	void CommandPoolSystem::Init(RenderDevice& device)
	{
		if (!m_Pools.empty())
			return;

		D_CORE_ASSERT(JobSystem::IsInitialized(), "JobSystem::Init must run before the command pools are created");
		if (!JobSystem::IsInitialized())
			return;

		const uint32_t threadCount = JobSystem::ThreadCount();
		D_CORE_ASSERT(threadCount > 0, "JobSystem has no threads");
		if (threadCount == 0)
			return;

		CommandPoolState poolState;
		poolState.resetBuffers = true;

		m_Pools.resize(threadCount);
		for (uint32_t i = 0; i < threadCount; i++)
			m_Pools[i] = CommandPool::Create(device, poolState);

		D_CORE_INFO("Command pools: {0}", threadCount);
	}

	void CommandPoolSystem::Shutdown()
	{
		m_Pools.clear();
	}

	uint32_t CommandPoolSystem::GetCount()
	{
		return static_cast<uint32_t>(m_Pools.size());
	}

	const Ref<CommandPool>& CommandPoolSystem::Get(uint32_t threadIndex)
	{
		if (threadIndex >= m_Pools.size())
		{
			D_CORE_ASSERT(false, "Command pool thread index is out of range");
			return m_Empty;
		}

		return m_Pools[threadIndex];
	}

	const Ref<CommandPool>& CommandPoolSystem::GetCurrent()
	{
		const uint32_t threadIndex = JobSystem::ThreadIndex();
		if (threadIndex == JobSystem::InvalidThread)
		{
			D_CORE_ASSERT(false, "This thread has no command pool");
			return m_Empty;
		}

		return Get(threadIndex);
	}
}
