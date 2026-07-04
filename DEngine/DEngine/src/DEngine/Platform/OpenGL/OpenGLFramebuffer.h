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

		void Invalidate();
		virtual const FramebufferSpecification& GetSpecification() const { return m_Specification; }

		virtual uint32_t GetColorAttachmentRendererID() const { return m_ColorAttachment; }
	private:
		uint32_t m_RendererID;
		uint32_t m_ColorAttachment;
		uint32_t m_DepthAttachment;
		FramebufferSpecification m_Specification;
	};
}
