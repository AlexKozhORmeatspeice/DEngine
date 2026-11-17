#include "dpch.h"
#include "OpenGLRenderPass.h"
#include "DEngine/SwapChain/SwapChain.h"
#include "DEngine/Renderer/CommandBuffer.h"
#include "DEngine/Renderer/Buffers/Framebuffer.h"

namespace DEngine
{
	OpenGLRenderPass::OpenGLRenderPass(RenderDevice& device, SwapChain& swapChain)
		: RenderPass(device, swapChain)
	{
	}

	void OpenGLRenderPass::Begin(CommandBuffer& commandBuffer, Framebuffer& framebuffer, const RenderPassState& state)
	{
	}

	void OpenGLRenderPass::End(CommandBuffer& commandBuffer)
	{
	}
}
