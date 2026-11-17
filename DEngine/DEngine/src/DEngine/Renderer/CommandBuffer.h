#pragma once

#include "DEngine/Core.h"
#include "DEngine/Renderer/Device/RenderDevice.h"

namespace DEngine
{
	enum class DCommandBufferLevel
	{
		Primary,
		Secondary
	};

	struct CommandBufferState
	{
		DCommandBufferLevel level = DCommandBufferLevel::Primary;
		bool oneTimeSubmit = false;
	};

	class D_API CommandBuffer
	{
	public:
		virtual ~CommandBuffer() = default;

		static Ref<CommandBuffer> Create(RenderDevice& device, const CommandBufferState& state);

		CommandBuffer(const CommandBuffer&) = delete;
		void operator=(const CommandBuffer&) = delete;

		virtual void Reset() = 0;
		virtual void Begin() = 0;
		virtual void End() = 0;

		const CommandBufferState& GetState() const { return m_State; }

	protected:
		explicit CommandBuffer(RenderDevice& device, const CommandBufferState& state)
			: m_State(state)
		{
		}

		CommandBufferState m_State;
	};
}
