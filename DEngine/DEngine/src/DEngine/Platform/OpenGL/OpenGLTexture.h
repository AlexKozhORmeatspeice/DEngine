#pragma once

#include "DEngine/Renderer/Texture/Texture.h"
#include "string"
#include "DEngine/Core/DataBuffer.h"

#include "glad/glad.h"

namespace DEngine
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
	public:
		OpenGLTexture2D(const TextureSpecification& spec, const DataBuffer& data = DataBuffer());
		virtual ~OpenGLTexture2D();

		virtual const TextureSpecification& GetSpecification() const { return m_Specification; }

		virtual unsigned int GetWidth() const override { return m_Width; }
		virtual unsigned int GetHeight() const override { return m_Height; }

		virtual void SetData(const DataBuffer& data) override;
		virtual	uint32_t GetRendererID() const override;

		virtual void Bind(CommandBuffer& commandBuffer, uint32_t slot = 0) const override;
		virtual void Unbind(CommandBuffer& commandBuffer) const override;
		virtual void* GetImGuiHandle() const override { return (void*)(uintptr_t)m_RendererID; }
	private:

		TextureSpecification m_Specification;

		unsigned int m_RendererID;
		std::string m_Path;
		unsigned int m_Width, m_Height;
		GLenum m_InternalFormat, m_DataFormat;
	};
}
