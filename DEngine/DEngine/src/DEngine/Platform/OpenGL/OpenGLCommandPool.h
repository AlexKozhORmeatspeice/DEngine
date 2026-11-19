#pragma once

#include "DEngine/Renderer/CommandPool.h"

namespace DEngine
{
	class D_API OpenGLCommandPool : public CommandPool
	{
	public:
		OpenGLCommandPool(RenderDevice& device, const CommandPoolState& state);
		~OpenGLCommandPool() override = default;

		void Reset() override;

	protected:
		Ref<CommandBuffer> AllocateNative(const CommandBufferState& state) override;

	private:
		RenderDevice& m_Device;
	};
}
