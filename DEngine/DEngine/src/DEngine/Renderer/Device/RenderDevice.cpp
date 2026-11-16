#include "dpch.h"
#include "RenderDevice.h"

#include "DEngine/Renderer/Renderer.h"
#include "DEngine/Platform/OpenGL/OpenGLRenderDevice.h"
#include "DEngine/Platform/Vulkan/VulkanRenderDevice.h"

namespace DEngine
{
	Scope<RenderDevice> RenderDevice::Create(Window& window)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::OpenGL: return CreateScope<OpenGLRenderDevice>(window);
			case RendererAPI::API::Vulkan: return CreateScope<VulkanRenderDevice>(window);
			case RendererAPI::API::None: D_CORE_ASSERT(false, "None API is not supported");
			default:
				break;
		}

		D_CORE_ASSERT(false, "Current Renderer API doesn't support a render device");
		return nullptr;
	}
}
