#pragma once

#include "DEngine/Core.h"
#include "DEngine/Renderer/Device/RenderDevice.h"

#include "vector"

namespace DEngine
{
	class SwapChain;
	class CommandBuffer;
	class Framebuffer;

	enum class DClearType
	{
		Color,
		DepthStencil
	};

	enum class DSubpassContents
	{
		Inline,
		Secondary
	};

	struct DClearValue
	{
		DClearType type = DClearType::Color;
		float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float depth = 1.0f;
		uint32_t stencil = 0;
	};

	struct RenderPassState
	{
		int32_t offsetX = 0;
		int32_t offsetY = 0;
		uint32_t width = 0;
		uint32_t height = 0;
		std::vector<DClearValue> clearValues;
		DSubpassContents contents = DSubpassContents::Inline;
	};

	class D_API RenderPass
	{
	public:
		virtual ~RenderPass() = default;

		static Ref<RenderPass> Create(RenderDevice& device, SwapChain& swapChain);

		RenderPass(const RenderPass&) = delete;
		void operator=(const RenderPass&) = delete;

		virtual void Begin(CommandBuffer& commandBuffer, Framebuffer& framebuffer, const RenderPassState& state) = 0;
		virtual void End(CommandBuffer& commandBuffer) = 0;

	protected:
		RenderPass() = default;
		explicit RenderPass(RenderDevice& device, SwapChain& swapChain)
		{
		}
	};
}
