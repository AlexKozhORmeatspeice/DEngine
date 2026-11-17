#include "dpch.h"
#include "UniformBuffer.h"

#include "DEngine/Renderer/Renderer.h"
#include "DEngine/Platform/OpenGL/OpenGLUniformBuffer.h"
#include "DEngine/Platform/Vulkan/VulkanUniformBuffer.h"

namespace DEngine
{
	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::OpenGL: return CreateRef<OpenGLUniformBuffer>(size);
			case RendererAPI::API::Vulkan: return CreateRef<VulkanUniformBuffer>(size);
			case RendererAPI::API::None: D_CORE_ASSERT(false, "None API is not supported");
			default:
				break;
		}

		D_CORE_ASSERT(false, "Current Renderer API doesn't support a uniform buffer");
		return nullptr;
	}
}
