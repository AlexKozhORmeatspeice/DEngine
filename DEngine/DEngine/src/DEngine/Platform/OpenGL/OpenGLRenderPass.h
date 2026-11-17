#pragma once

#include "DEngine/Renderer/Pipeline/RenderPass.h"

namespace DEngine
{
	class D_API OpenGLRenderPass : public RenderPass
	{
	public:
		explicit OpenGLRenderPass(RenderDevice& device, SwapChain& swapChain);
		~OpenGLRenderPass() override = default;

		void Begin(CommandBuffer& commandBuffer, Framebuffer& framebuffer, const RenderPassState& state) override;
		void End(CommandBuffer& commandBuffer) override;
	};
}
