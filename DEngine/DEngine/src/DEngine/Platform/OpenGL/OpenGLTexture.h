#pragma once

#include "DEngine/Renderer/Texture/Texture.h"
#include "string"

namespace DEngine
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const std::string& path);
		virtual ~OpenGLTexture2D();

		virtual unsigned int GetWidth() const override { return m_Width; }
		virtual unsigned int GetHeight() const override { return m_Height; }

		virtual void SetData(void* data, uint32_t size) override;
		virtual	uint32_t GetRendererID() const override;

		virtual void Bind(unsigned int slot = 0) const override;
	private:
		unsigned int m_RendererID;
		std::string m_Path;
		unsigned int m_Width, m_Height;
	};
}
