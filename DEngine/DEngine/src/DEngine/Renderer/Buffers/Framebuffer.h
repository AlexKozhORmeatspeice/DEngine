#pragma once

#include "DEngine/Core.h"
#include "DEngine/Renderer/CommandBuffer.h"
#include "DEngine/Renderer/Pipeline/RenderPass.h"

namespace DEngine
{
	struct FramebufferState
	{
		uint32_t Width = 0;
		uint32_t Height = 0;
		uint32_t samples = 0;
		uint32_t layers = 1;
		bool SwapChainTarget = false;
	};

	using FramebufferSpecification = FramebufferState;

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;
		virtual void Bind(CommandBuffer& commandBuffer) = 0;
		virtual void Unbind(CommandBuffer& commandBuffer) = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual const FramebufferSpecification& GetSpecification() const = 0;
		virtual uint32_t GetColorAttachmentRendererID() const = 0;
		virtual const Ref<RenderPass>& GetRenderPass() const { return m_RenderPass; }
		virtual void* GetImGuiHandle() const { return (void*)(uintptr_t)GetColorAttachmentRendererID(); }
		void SetRenderPass(const Ref<RenderPass>& renderPass) { m_RenderPass = renderPass; }

		static Ref<Framebuffer> Create(const FramebufferSpecification& spec);
	protected:
		Ref<RenderPass> m_RenderPass;
	};
}
