#include "dpch.h"
#include "OpenGLCommandPool.h"

#include "DEngine/Platform/OpenGL/OpenGLCommandBuffer.h"

namespace DEngine
{
	OpenGLCommandPool::OpenGLCommandPool(RenderDevice& device, const CommandPoolState& state)
		: m_Device(device)
	{
		(void)state;
	}

	void OpenGLCommandPool::Reset()
	{
		for (Ref<CommandBuffer>& buffer : m_Buffers)
		{
			if (buffer)
				buffer->Reset();
		}
	}

	Ref<CommandBuffer> OpenGLCommandPool::AllocateNative(const CommandBufferState& state)
	{
		return CreateRef<OpenGLCommandBuffer>(m_Device, state);
	}
}
