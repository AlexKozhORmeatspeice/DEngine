#pragma once

#include "DEngine/Renderer/Texture/Texture.h"
#include "string"
#include "DEngine/Core/DataBuffer.h"

namespace DEngine
{
	class VulkanTexture2D : public Texture2D
	{
	public:
		VulkanTexture2D(const TextureSpecification& spec, const DataBuffer& data = DataBuffer());
		virtual ~VulkanTexture2D();

		virtual const TextureSpecification& GetSpecification() const { return m_Specification; }

		virtual unsigned int GetWidth() const override { return m_Width; }
		virtual unsigned int GetHeight() const override { return m_Height; }

		virtual void SetData(const DataBuffer& data) override;
		virtual uint32_t GetRendererID() const override;

		virtual void Bind(unsigned int slot = 0) const override;
	private:
		TextureSpecification m_Specification;

		unsigned int m_RendererID;
		std::string m_Path;
		unsigned int m_Width, m_Height;
		uint32_t m_Format;
	};
}
