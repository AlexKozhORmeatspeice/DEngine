#include "dpch.h"
#include "VulkanTexture.h"

namespace DEngine
{
	VulkanTexture2D::VulkanTexture2D(const TextureSpecification& spec, const DataBuffer& data)
		: m_Specification(spec)
	{
	}

	VulkanTexture2D::~VulkanTexture2D()
	{
	}

	void VulkanTexture2D::SetData(const DataBuffer& data)
	{
	}

	uint32_t VulkanTexture2D::GetRendererID() const
	{
		return m_RendererID;
	}

	void VulkanTexture2D::Bind(unsigned int slot) const
	{
	}
}
