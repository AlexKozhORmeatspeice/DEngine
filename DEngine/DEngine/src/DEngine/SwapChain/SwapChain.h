#pragma once

#include "DEngine/Core.h"
#include "DEngine/Renderer/Device/RenderDevice.h"
#include "DEngine/Renderer/Buffers/Framebuffer.h"

#include "vector"

namespace DEngine
{
	class RenderPass;
	class CommandBuffer;

	class D_API SwapChain
	{
	public:
		virtual ~SwapChain() = default;

		static Scope<SwapChain> Create(RenderDevice& device, uint32_t width, uint32_t height);

		SwapChain(const SwapChain&) = delete;
		void operator=(const SwapChain&) = delete;

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }
		uint32_t GetImageCount() const { return m_ImageCount; }
		virtual uint32_t GetCurrentFrame() const { return 0; }

		float GetExtentAspectRatio() const
		{
			return static_cast<float>(m_Width) / static_cast<float>(m_Height);
		}

		const Ref<RenderPass>& GetRenderPass() const { return m_RenderPass; }
		const Ref<RenderPass>& GetContinueRenderPass() const { return m_ContinueRenderPass ? m_ContinueRenderPass : m_RenderPass; }
		const Ref<Framebuffer>& GetFramebuffer(uint32_t index) const { return m_Framebuffers[index]; }

		virtual bool AcquireNextImage(uint32_t& imageIndex);
		virtual bool Submit(CommandBuffer& commandBuffer, uint32_t imageIndex);
		virtual void Resize(uint32_t width, uint32_t height) {}

	protected:
		SwapChain() = default;
		explicit SwapChain(RenderDevice& device, uint32_t width, uint32_t height)
			: m_Width(width)
			, m_Height(height)
		{
		}

		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		uint32_t m_ImageCount = 0;
		Ref<RenderPass> m_RenderPass;
		Ref<RenderPass> m_ContinueRenderPass;
		std::vector<Ref<Framebuffer>> m_Framebuffers;
	};
}
