#include "dpch.h"
#include "CommandBuffer.h"

#include "DEngine/Renderer/Renderer.h"
#include "DEngine/Platform/OpenGL/OpenGLCommandBuffer.h"
#include "DEngine/Platform/Vulkan/VulkanCommandBuffer.h"

namespace DEngine
{
	Ref<CommandBuffer> CommandBuffer::Create(RenderDevice& device, const CommandBufferState& state)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::OpenGL: return CreateRef<OpenGLCommandBuffer>(device, state);
			case RendererAPI::API::Vulkan: return CreateRef<VulkanCommandBuffer>(device, state);
			case RendererAPI::API::None: D_CORE_ASSERT(false, "None API is not supported");
			default:
				break;
		}

		D_CORE_ASSERT(false, "Current Renderer API doesn't support a command buffer");
		return nullptr;
	}
}
