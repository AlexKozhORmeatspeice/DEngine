#pragma once

#include "DEngine/SwapChain/SwapChain.h"
#include "DEngine/Platform/Vulkan/VulkanRenderDevice.h"

#include "vector"

namespace DEngine
{
	class D_API VulkanSwapChain : public SwapChain
	{
	public:
		static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

		explicit VulkanSwapChain(RenderDevice& device, uint32_t width, uint32_t height);
		~VulkanSwapChain() override;

		VulkanSwapChain(const VulkanSwapChain&) = delete;
		void operator=(const VulkanSwapChain&) = delete;

		VkFramebuffer GetFrameBuffer(int index) { return m_SwapChainFramebuffers[index]; }
		VkRenderPass GetRenderPass() { return m_RenderPass; }
		VkImageView GetImageView(int index) { return m_SwapChainImageViews[index]; }
		VkFormat GetSwapChainImageFormat() { return m_SwapChainImageFormat; }
		VkExtent2D GetSwapChainExtent() { return m_SwapChainExtent; }

		VkFormat FindDepthFormat();

		VkResult AcquireNextImage(uint32_t* imageIndex);
		VkResult SubmitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);

	private:
		void CreateSwapChain();
		void CreateImageViews();
		void CreateDepthResources();
		void CreateRenderPass();
		void CreateFramebuffers();
		void CreateSyncObjects();

		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		VkFormat m_SwapChainImageFormat;
		VkExtent2D m_SwapChainExtent;

		std::vector<VkFramebuffer> m_SwapChainFramebuffers;
		VkRenderPass m_RenderPass = VK_NULL_HANDLE;

		std::vector<VkImage> m_DepthImages;
		std::vector<VkDeviceMemory> m_DepthImageMemory;
		std::vector<VkImageView> m_DepthImageViews;
		std::vector<VkImage> m_SwapChainImages;
		std::vector<VkImageView> m_SwapChainImageViews;

		VulkanRenderDevice& m_Device;
		VkExtent2D m_WindowExtent;

		VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;

		std::vector<VkSemaphore> m_ImageAvailableSemaphores;
		std::vector<VkSemaphore> m_RenderFinishedSemaphores;
		std::vector<VkFence> m_InFlightFences;
		std::vector<VkFence> m_ImagesInFlight;
		size_t m_CurrentFrame = 0;
	};
}
