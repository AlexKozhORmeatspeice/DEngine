#include "dpch.h"
#include "VulkanFramebuffer.h"

#include "DEngine/Application.h"
#include "DEngine/Platform/Vulkan/VulkanImGuiHelper.h"
#include "DEngine/Platform/Vulkan/VulkanRenderDevice.h"
#include "DEngine/Platform/Vulkan/VulkanRenderPass.h"
#include "DEngine/Platform/Vulkan/VulkanSwapChain.h"

#include "array"

namespace DEngine
{
	static VkImageView CreateView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspect)
	{
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = aspect;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.layerCount = 1;

		VkImageView view = VK_NULL_HANDLE;
		D_CORE_ASSERT(vkCreateImageView(device, &viewInfo, nullptr, &view) == VK_SUCCESS, "Failed to create framebuffer image view");
		return view;
	}

	VulkanFramebuffer::VulkanFramebuffer(const FramebufferSpecification& spec)
		: m_Specification(spec)
		, m_Device(*static_cast<VulkanRenderDevice&>(Application::Get().GetDevice()).Device())
		, m_Offscreen(true)
	{
		CreateOffscreen();
	}

	VulkanFramebuffer::VulkanFramebuffer(RenderDevice& device, const FramebufferState& state, VkRenderPass renderPass, VkImageView colorView, VkImageView depthView)
		: m_Specification(state)
		, m_Device(*static_cast<VulkanRenderDevice&>(device).Device())
		, m_ColorView(colorView)
		, m_DepthView(depthView)
	{
		std::array<VkImageView, 2> attachments = { colorView, depthView };

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = state.Width;
		framebufferInfo.height = state.Height;
		framebufferInfo.layers = state.layers;

		D_CORE_ASSERT(vkCreateFramebuffer(m_Device, &framebufferInfo, nullptr, &m_Framebuffer) == VK_SUCCESS, "failed to create framebuffer!");
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
		ReleaseOffscreen();
		if (!m_Offscreen && m_Framebuffer != VK_NULL_HANDLE && m_Device != VK_NULL_HANDLE)
			vkDestroyFramebuffer(m_Device, m_Framebuffer, nullptr);
	}

	void VulkanFramebuffer::CreateOffscreen()
	{
		if (m_Specification.Width == 0 || m_Specification.Height == 0)
			return;

		VulkanSwapChain& swapChain = static_cast<VulkanSwapChain&>(Application::Get().GetSwapChain());
		const VkFormat colorFormat = swapChain.GetSwapChainImageFormat();
		const VkFormat depthFormat = swapChain.FindDepthFormat();
		const bool depthStencil = depthFormat == VK_FORMAT_D32_SFLOAT_S8_UINT || depthFormat == VK_FORMAT_D24_UNORM_S8_UINT;
		const VkImageAspectFlags depthAspect = depthStencil ? (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT) : VK_IMAGE_ASPECT_DEPTH_BIT;

		VkImageCreateInfo colorInfo{};
		colorInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		colorInfo.imageType = VK_IMAGE_TYPE_2D;
		colorInfo.extent = { m_Specification.Width, m_Specification.Height, 1 };
		colorInfo.mipLevels = 1;
		colorInfo.arrayLayers = 1;
		colorInfo.format = colorFormat;
		colorInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		colorInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		colorInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		colorInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		m_ColorImage = VulkanRendererAPI::Get().AllocateImage(colorInfo);
		m_ColorView = CreateView(m_Device, m_ColorImage.Image, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);

		VkImageCreateInfo depthInfo = colorInfo;
		depthInfo.format = depthFormat;
		depthInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		m_DepthImage = VulkanRendererAPI::Get().AllocateImage(depthInfo);
		m_DepthView = CreateView(m_Device, m_DepthImage.Image, depthFormat, depthAspect);

		m_RenderPass = CreateRef<VulkanRenderPass>(Application::Get().GetDevice(), colorFormat, depthFormat, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		std::array<VkImageView, 2> attachments = { m_ColorView, m_DepthView };
		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = static_cast<VulkanRenderPass*>(m_RenderPass.get())->GetNative();
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = m_Specification.Width;
		framebufferInfo.height = m_Specification.Height;
		framebufferInfo.layers = m_Specification.layers == 0 ? 1 : m_Specification.layers;
		D_CORE_ASSERT(vkCreateFramebuffer(m_Device, &framebufferInfo, nullptr, &m_Framebuffer) == VK_SUCCESS, "Failed to create offscreen framebuffer");
	}

	void VulkanFramebuffer::ReleaseOffscreen()
	{
		if (!m_Offscreen || m_Device == VK_NULL_HANDLE)
			return;

		if (m_Framebuffer != VK_NULL_HANDLE)
			vkDestroyFramebuffer(m_Device, m_Framebuffer, nullptr);
		if (m_ColorView != VK_NULL_HANDLE)
			vkDestroyImageView(m_Device, m_ColorView, nullptr);
		if (m_DepthView != VK_NULL_HANDLE)
			vkDestroyImageView(m_Device, m_DepthView, nullptr);
		m_Framebuffer = VK_NULL_HANDLE;
		m_ColorView = VK_NULL_HANDLE;
		m_DepthView = VK_NULL_HANDLE;
		m_ImGuiHandle = nullptr;

		if (VulkanRendererAPI::TryGet())
		{
			VulkanRendererAPI::Get().FreeImage(m_ColorImage);
			VulkanRendererAPI::Get().FreeImage(m_DepthImage);
		}
		m_RenderPass = nullptr;
	}

	void VulkanFramebuffer::Bind(CommandBuffer& commandBuffer)
	{
	}

	void VulkanFramebuffer::Unbind(CommandBuffer& commandBuffer)
	{
	}

	void VulkanFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		if (!m_Offscreen || (m_Specification.Width == width && m_Specification.Height == height))
			return;

		m_Specification.Width = width;
		m_Specification.Height = height;
		if (width == 0 || height == 0)
			return;

		vkDeviceWaitIdle(m_Device);
		ReleaseOffscreen();
		CreateOffscreen();
	}

	void* VulkanFramebuffer::GetImGuiHandle() const
	{
		if (!m_Offscreen)
			return nullptr;
		if (!m_ImGuiHandle && VulkanImGuiHelper::IsReady())
			m_ImGuiHandle = VulkanImGuiHelper::RegisterTexture(m_ColorView, nullptr);
		return m_ImGuiHandle;
	}

	void VulkanFramebuffer::Invalidate()
	{
		Resize(m_Specification.Width, m_Specification.Height);
	}
}
