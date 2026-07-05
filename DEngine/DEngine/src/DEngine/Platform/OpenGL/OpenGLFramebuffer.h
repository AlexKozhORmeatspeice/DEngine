#pragma once

#include "DEngine/Renderer/Buffers/Framebuffer.h"

namespace DEngine
{
	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecification& spec);
		virtual ~OpenGLFramebuffer();

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void Resize(uint32_t width, uint32_t height) override;

		void Invalidate();
		virtual const FramebufferSpecification& GetSpecification() const { return m_Specification; }

		virtual uint32_t GetColorAttachmentRendererID() const { return m_ColorAttachment; }
	private:
		uint32_t m_RendererID = 0;
		uint32_t m_ColorAttachment = 0;
		uint32_t m_DepthAttachment = 0;
		FramebufferSpecification m_Specification;
	};
}
