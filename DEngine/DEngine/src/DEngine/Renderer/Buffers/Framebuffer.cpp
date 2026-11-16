#include "dpch.h"
#include "Framebuffer.h"

#include "DEngine/Renderer/Renderer.h"
#include "DEngine/Platform/OpenGL/OpenGLFramebuffer.h"
#include "DEngine/Platform/Vulkan/VulkanFramebuffer.h"

namespace DEngine
{
	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::OpenGL: return std::make_shared<OpenGLFramebuffer>(spec);
			case RendererAPI::API::Vulkan: return std::make_shared<VulkanFramebuffer>(spec);
			case RendererAPI::API::None: D_CORE_ASSERT(false, "None API is not supported");
			default:
				break;
		}

		D_CORE_ASSERT(false, "Current Renderer API doesn't support frame buffers");
		return nullptr;

	}
}