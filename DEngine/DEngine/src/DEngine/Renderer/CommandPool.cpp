#include "dpch.h"
#include "CommandPool.h"

#include "DEngine/Renderer/Renderer.h"
#include "DEngine/Platform/OpenGL/OpenGLCommandPool.h"
#include "DEngine/Platform/Vulkan/VulkanCommandPool.h"

namespace DEngine
{
	Ref<CommandPool> CommandPool::Create(RenderDevice& device, const CommandPoolState& state)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::OpenGL: return CreateRef<OpenGLCommandPool>(device, state);
			case RendererAPI::API::Vulkan: return CreateRef<VulkanCommandPool>(device, state);
			case RendererAPI::API::None: D_CORE_ASSERT(false, "None API is not supported");
			default:
				break;
		}

		D_CORE_ASSERT(false, "Current Renderer API doesn't support a command pool");
		return nullptr;
	}

	Ref<CommandBuffer> CommandPool::Allocate(const CommandBufferState& state)
	{
		Ref<CommandBuffer> buffer = AllocateNative(state);
		if (buffer)
			m_Buffers.push_back(buffer);
		return buffer;
	}

	Ref<CommandBuffer> CommandPool::AllocateSecondary(bool oneTimeSubmit)
	{
		CommandBufferState state;
		state.level = DCommandBufferLevel::Secondary;
		state.oneTimeSubmit = oneTimeSubmit;
		return Allocate(state);
	}

	void CommandPool::FreeAll()
	{
		std::vector<Ref<CommandBuffer>> stillHeld;
		stillHeld.reserve(m_Buffers.size());
		for (const Ref<CommandBuffer>& buffer : m_Buffers)
		{
			if (buffer && buffer.use_count() > 1)
				stillHeld.push_back(buffer);
		}
		m_Buffers.swap(stillHeld);
	}
}
