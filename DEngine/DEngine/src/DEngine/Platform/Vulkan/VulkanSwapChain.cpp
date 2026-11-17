#include "dpch.h"
#include "VulkanSwapChain.h"
#include "DEngine/Renderer/Pipeline/RenderPass.h"
#include "DEngine/Platform/Vulkan/VulkanRenderPass.h"
#include "DEngine/Platform/Vulkan/VulkanFramebuffer.h"
#include "DEngine/Platform/Vulkan/VulkanCommandBuffer.h"
#include "DEngine/Renderer/CommandBuffer.h"

#include "algorithm"
#include "limits"

namespace DEngine
{
	VulkanSwapChain::VulkanSwapChain(RenderDevice& device, uint32_t width, uint32_t height)
		: SwapChain(device, width, height)
		, m_Device(static_cast<VulkanRenderDevice&>(device))
		, m_WindowExtent{ width, height }
	{
		CreateSwapChain();
		CreateImageViews();
		CreateRenderPass();
		CreateDepthResources();
		CreateFramebuffers();
		CreateSyncObjects();
	}

	VulkanSwapChain::~VulkanSwapChain()
	{
		VkDevice device = *m_Device.Device();

		m_Framebuffers.clear();

		for (auto imageView : m_SwapChainImageViews)
			vkDestroyImageView(device, imageView, nullptr);
		m_SwapChainImageViews.clear();

		if (m_SwapChain != VK_NULL_HANDLE)
		{
			vkDestroySwapchainKHR(device, m_SwapChain, nullptr);
			m_SwapChain = VK_NULL_HANDLE;
		}

		for (size_t i = 0; i < m_DepthImages.size(); i++)
		{
			vkDestroyImageView(device, m_DepthImageViews[i], nullptr);
			vkDestroyImage(device, m_DepthImages[i], nullptr);
			vkFreeMemory(device, m_DepthImageMemory[i], nullptr);
		}

		for (VkSemaphore semaphore : m_RenderFinishedSemaphores)
		{
			if (semaphore != VK_NULL_HANDLE)
				vkDestroySemaphore(device, semaphore, nullptr);
		}

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkDestroySemaphore(device, m_ImageAvailableSemaphores[i], nullptr);
			vkDestroyFence(device, m_InFlightFences[i], nullptr);
		}
	}

	bool VulkanSwapChain::AcquireNextImage(uint32_t& imageIndex)
	{
		VkDevice device = *m_Device.Device();

		vkWaitForFences(
			device,
			1,
			&m_InFlightFences[m_CurrentFrame],
			VK_TRUE,
			std::numeric_limits<uint64_t>::max());

		VkResult result = vkAcquireNextImageKHR(
			device,
			m_SwapChain,
			std::numeric_limits<uint64_t>::max(),
			m_ImageAvailableSemaphores[m_CurrentFrame],
			VK_NULL_HANDLE,
			&imageIndex);

		if ((result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR) && imageIndex < m_ImagesInFlight.size() && m_ImagesInFlight[imageIndex] != VK_NULL_HANDLE)
			vkWaitForFences(device, 1, &m_ImagesInFlight[imageIndex], VK_TRUE, std::numeric_limits<uint64_t>::max());

		return result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR;
	}

	void VulkanSwapChain::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0)
			return;

		VkDevice device = *m_Device.Device();
		vkDeviceWaitIdle(device);

		m_Framebuffers.clear();

		for (auto imageView : m_SwapChainImageViews)
			vkDestroyImageView(device, imageView, nullptr);
		m_SwapChainImageViews.clear();
		m_SwapChainImages.clear();

		if (m_SwapChain != VK_NULL_HANDLE)
		{
			vkDestroySwapchainKHR(device, m_SwapChain, nullptr);
			m_SwapChain = VK_NULL_HANDLE;
		}

		for (size_t i = 0; i < m_DepthImages.size(); i++)
		{
			vkDestroyImageView(device, m_DepthImageViews[i], nullptr);
			vkDestroyImage(device, m_DepthImages[i], nullptr);
			vkFreeMemory(device, m_DepthImageMemory[i], nullptr);
		}
		m_DepthImages.clear();
		m_DepthImageMemory.clear();
		m_DepthImageViews.clear();

		m_Width = width;
		m_Height = height;
		m_WindowExtent = { width, height };

		CreateSwapChain();
		CreateImageViews();
		if (!m_RenderPass)
			CreateRenderPass();
		CreateDepthResources();
		CreateFramebuffers();
		EnsurePresentSemaphores();

		if (m_ImagesInFlight.size() != m_ImageCount)
			m_ImagesInFlight.assign(m_ImageCount, VK_NULL_HANDLE);
	}

	bool VulkanSwapChain::Submit(CommandBuffer& commandBuffer, uint32_t imageIndex)
	{
		VkDevice device = *m_Device.Device();
		VkCommandBuffer nativeCommand = static_cast<VulkanCommandBuffer&>(commandBuffer).GetNative();

		if (m_ImagesInFlight[imageIndex] != VK_NULL_HANDLE)
			vkWaitForFences(device, 1, &m_ImagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
		m_ImagesInFlight[imageIndex] = m_InFlightFences[m_CurrentFrame];

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphores[m_CurrentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &nativeCommand;

		VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphores[imageIndex] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		vkResetFences(device, 1, &m_InFlightFences[m_CurrentFrame]);
		if (vkQueueSubmit(m_Device.GraphicsQueue(), 1, &submitInfo, m_InFlightFences[m_CurrentFrame]) != VK_SUCCESS)
		{
			D_CORE_ASSERT(false, "failed to submit draw command buffer!");
			return false;
		}

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { m_SwapChain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;

		presentInfo.pImageIndices = &imageIndex;

		VkResult result = vkQueuePresentKHR(m_Device.PresentQueue(), &presentInfo);

		m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

		return result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR;
	}

	void VulkanSwapChain::CreateSwapChain()
	{
		SwapChainSupportDetails swapChainSupport = m_Device.GetSwapChainSupport();

		VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
			imageCount = swapChainSupport.capabilities.maxImageCount;

		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_Device.Surface();

		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamilyIndices indices = m_Device.FindPhysicalQueueFamilies();
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily, indices.presentFamily };

		if (indices.graphicsFamily != indices.presentFamily)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}

		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		createInfo.oldSwapchain = VK_NULL_HANDLE;

		D_CORE_ASSERT(vkCreateSwapchainKHR(*m_Device.Device(), &createInfo, nullptr, &m_SwapChain) == VK_SUCCESS, "failed to create swap chain!");

		vkGetSwapchainImagesKHR(*m_Device.Device(), m_SwapChain, &imageCount, nullptr);
		m_SwapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(*m_Device.Device(), m_SwapChain, &imageCount, m_SwapChainImages.data());

		m_SwapChainImageFormat = surfaceFormat.format;
		m_SwapChainExtent = extent;
		m_Width = extent.width;
		m_Height = extent.height;
		m_ImageCount = imageCount;
	}

	void VulkanSwapChain::CreateImageViews()
	{
		VkDevice device = *m_Device.Device();

		m_SwapChainImageViews.resize(m_SwapChainImages.size());
		for (size_t i = 0; i < m_SwapChainImages.size(); i++)
		{
			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = m_SwapChainImages[i];
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = m_SwapChainImageFormat;
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			D_CORE_ASSERT(vkCreateImageView(device, &viewInfo, nullptr, &m_SwapChainImageViews[i]) == VK_SUCCESS, "failed to create texture image view!");
		}
	}

	void VulkanSwapChain::CreateRenderPass()
	{
		m_RenderPass = RenderPass::Create(m_Device, *this);
	}

	void VulkanSwapChain::CreateFramebuffers()
	{
		m_Framebuffers.resize(GetImageCount());
		for (size_t i = 0; i < GetImageCount(); i++)
		{
			FramebufferState state;
			state.Width = m_SwapChainExtent.width;
			state.Height = m_SwapChainExtent.height;
			state.layers = 1;
			state.SwapChainTarget = true;
			m_Framebuffers[i] = CreateRef<VulkanFramebuffer>(m_Device, state, static_cast<VulkanRenderPass*>(m_RenderPass.get())->GetNative(), m_SwapChainImageViews[i], m_DepthImageViews[i]);
			m_Framebuffers[i]->SetRenderPass(m_RenderPass);
		}
	}

	void VulkanSwapChain::CreateDepthResources()
	{
		VkFormat depthFormat = FindDepthFormat();
		VkExtent2D swapChainExtent = GetExtent();

		m_DepthImages.resize(GetImageCount());
		m_DepthImageMemory.resize(GetImageCount());
		m_DepthImageViews.resize(GetImageCount());

		for (size_t i = 0; i < m_DepthImages.size(); i++)
		{
			VkImageCreateInfo imageInfo{};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent.width = swapChainExtent.width;
			imageInfo.extent.height = swapChainExtent.height;
			imageInfo.extent.depth = 1;
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.format = depthFormat;
			imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageInfo.flags = 0;

			m_Device.CreateImageWithInfo(
				imageInfo,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				m_DepthImages[i],
				m_DepthImageMemory[i]);

			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = m_DepthImages[i];
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = depthFormat;
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			D_CORE_ASSERT(vkCreateImageView(*m_Device.Device(), &viewInfo, nullptr, &m_DepthImageViews[i]) == VK_SUCCESS, "failed to create texture image view!");
		}
	}

	void VulkanSwapChain::CreateSyncObjects()
	{
		VkDevice device = *m_Device.Device();

		m_ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		m_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
		m_ImagesInFlight.resize(GetImageCount(), VK_NULL_HANDLE);

		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			D_CORE_ASSERT(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]) == VK_SUCCESS, "failed to create synchronization objects for a frame!");
			D_CORE_ASSERT(vkCreateFence(device, &fenceInfo, nullptr, &m_InFlightFences[i]) == VK_SUCCESS, "failed to create synchronization objects for a frame!");
		}

		EnsurePresentSemaphores();
	}

	void VulkanSwapChain::EnsurePresentSemaphores()
	{
		VkDevice device = *m_Device.Device();
		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		while (m_RenderFinishedSemaphores.size() > m_ImageCount)
		{
			vkDestroySemaphore(device, m_RenderFinishedSemaphores.back(), nullptr);
			m_RenderFinishedSemaphores.pop_back();
		}

		while (m_RenderFinishedSemaphores.size() < m_ImageCount)
		{
			VkSemaphore semaphore = VK_NULL_HANDLE;
			D_CORE_ASSERT(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &semaphore) == VK_SUCCESS, "failed to create present semaphore");
			m_RenderFinishedSemaphores.push_back(semaphore);
		}
	}

	VkSurfaceFormatKHR VulkanSwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto& availableFormat : availableFormats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				return availableFormat;
		}

		return availableFormats[0];
	}

	VkPresentModeKHR VulkanSwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		for (const auto& availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				D_CORE_INFO("Present mode: Mailbox");
				return availablePresentMode;
			}
		}

		D_CORE_INFO("Present mode: V-Sync");
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D VulkanSwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			return capabilities.currentExtent;

		VkExtent2D actualExtent = m_WindowExtent;
		actualExtent.width = std::max(
			capabilities.minImageExtent.width,
			std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(
			capabilities.minImageExtent.height,
			std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}

	VkFormat VulkanSwapChain::FindDepthFormat()
	{
		return m_Device.FindSupportedFormat(
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}
}
