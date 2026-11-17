#include "dpch.h"
#include "RenderPass.h"

#include "DEngine/Renderer/Renderer.h"
#include "DEngine/SwapChain/SwapChain.h"
#include "DEngine/Platform/OpenGL/OpenGLRenderPass.h"
#include "DEngine/Platform/Vulkan/VulkanRenderPass.h"
#include "DEngine/Platform/Vulkan/VulkanSwapChain.h"

namespace DEngine
{
	Ref<RenderPass> RenderPass::Create(RenderDevice& device, SwapChain& swapChain)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::OpenGL: return CreateRef<OpenGLRenderPass>(device, swapChain);
			case RendererAPI::API::Vulkan: return CreateRef<VulkanRenderPass>(device, static_cast<VulkanSwapChain&>(swapChain));
			case RendererAPI::API::None: D_CORE_ASSERT(false, "None API is not supported");
			default:
				break;
		}

		D_CORE_ASSERT(false, "Current Renderer API doesn't support a render pass");
		return nullptr;
	}
}
