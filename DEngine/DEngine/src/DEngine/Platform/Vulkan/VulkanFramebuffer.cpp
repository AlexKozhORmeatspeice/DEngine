#include "dpch.h"
#include "VulkanFramebuffer.h"

namespace DEngine
{
	VulkanFramebuffer::VulkanFramebuffer(const FramebufferSpecification& spec)
		: m_Specification(spec)
	{
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
	}

	void VulkanFramebuffer::Bind()
	{
	}

	void VulkanFramebuffer::Unbind()
	{
	}

	void VulkanFramebuffer::Resize(uint32_t width, uint32_t height)
	{
	}

	void VulkanFramebuffer::Invalidate()
	{
	}
}
