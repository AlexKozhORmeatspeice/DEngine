#include "dpch.h"
#include "SwapChain.h"

#include "DEngine/Renderer/Renderer.h"
#include "DEngine/Platform/OpenGL/OpenGLSwapChain.h"
#include "DEngine/Platform/Vulkan/VulkanSwapChain.h"

namespace DEngine
{
	Scope<SwapChain> SwapChain::Create(RenderDevice& device, uint32_t width, uint32_t height)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::OpenGL: return CreateScope<OpenGLSwapChain>(device, width, height);
			case RendererAPI::API::Vulkan: return CreateScope<VulkanSwapChain>(device, width, height);
			case RendererAPI::API::None: D_CORE_ASSERT(false, "None API is not supported");
			default:
				break;
		}

		D_CORE_ASSERT(false, "Current Renderer API doesn't support a swap chain");
		return nullptr;
	}
}
