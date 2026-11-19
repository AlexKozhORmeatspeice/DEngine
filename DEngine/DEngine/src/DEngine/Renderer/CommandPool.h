#pragma once

#include "DEngine/Core.h"
#include "DEngine/Renderer/CommandBuffer.h"

#include "vector"

namespace DEngine
{
	struct CommandPoolState
	{
		bool transient = false;
		bool resetBuffers = true;
	};

	// Recorded by one thread. CommandPoolSystem owns one pool per job thread.
	class D_API CommandPool
	{
	public:
		virtual ~CommandPool() = default;

		static Ref<CommandPool> Create(RenderDevice& device, const CommandPoolState& state);

		CommandPool(const CommandPool&) = delete;
		void operator=(const CommandPool&) = delete;

		Ref<CommandBuffer> Allocate(const CommandBufferState& state);
		Ref<CommandBuffer> AllocateSecondary(bool oneTimeSubmit = true);
		void FreeAll();
		virtual void Reset() = 0;

		const std::vector<Ref<CommandBuffer>>& GetBuffers() const { return m_Buffers; }

	protected:
		CommandPool() = default;

		virtual Ref<CommandBuffer> AllocateNative(const CommandBufferState& state) = 0;

		std::vector<Ref<CommandBuffer>> m_Buffers;
	};
}
