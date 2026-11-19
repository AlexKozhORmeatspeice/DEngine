#pragma once

#include "DEngine/Core.h"
#include "DEngine/Renderer/CommandPool.h"

namespace DEngine
{
	// One command pool per job thread. Thread 0 is the render thread.
	class D_API CommandPoolSystem
	{
	public:
		static void Init(RenderDevice& device);
		static void Shutdown();

		static uint32_t GetCount();
		static const Ref<CommandPool>& Get(uint32_t threadIndex);
		static const Ref<CommandPool>& GetCurrent();

	private:
		static std::vector<Ref<CommandPool>> m_Pools;
		static Ref<CommandPool> m_Empty;
	};
}
