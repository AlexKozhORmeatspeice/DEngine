#include "dpch.h"
#include "VulkanImGuiHelper.h"

#include "backends/imgui_impl_vulkan.h"

#include "DEngine/Application.h"
#include "DEngine/Platform/Vulkan/VulkanRenderDevice.h"
#include "DEngine/Platform/Vulkan/VulkanRenderPass.h"
#include "DEngine/Platform/Vulkan/VulkanSwapChain.h"
#include "DEngine/Platform/Vulkan/VulkanCommandBuffer.h"
#include "DEngine/Renderer/Renderer.h"

namespace DEngine
{
	static VkDescriptorPool s_Pool = VK_NULL_HANDLE;
	bool VulkanImGuiHelper::s_Ready = false;
	static VkSampler s_Sampler = VK_NULL_HANDLE;

	void VulkanImGuiHelper::Init()
	{
		VulkanRenderDevice& device = static_cast<VulkanRenderDevice&>(Application::Get().GetDevice());
		VulkanSwapChain& swapChain = static_cast<VulkanSwapChain&>(Application::Get().GetSwapChain());

		VkDescriptorPoolSize poolSize{};
		poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSize.descriptorCount = 1024;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		poolInfo.maxSets = 1024;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;
		D_CORE_ASSERT(vkCreateDescriptorPool(*device.Device(), &poolInfo, nullptr, &s_Pool) == VK_SUCCESS, "Failed to create ImGui descriptor pool");

		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.maxLod = 1.0f;
		D_CORE_ASSERT(vkCreateSampler(*device.Device(), &samplerInfo, nullptr, &s_Sampler) == VK_SUCCESS, "Failed to create ImGui sampler");

		ImGui_ImplVulkan_InitInfo info{};
		info.Instance = device.GetInstance();
		info.PhysicalDevice = device.GetPhysicalDevice();
		info.Device = *device.Device();
		info.QueueFamily = device.FindPhysicalQueueFamilies().graphicsFamily;
		info.Queue = device.GraphicsQueue();
		info.DescriptorPool = s_Pool;
		info.MinImageCount = 2;
		info.ImageCount = swapChain.GetImageCount() < 2 ? 2 : swapChain.GetImageCount();
		info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

		VkRenderPass renderPass = static_cast<VulkanRenderPass*>(swapChain.GetRenderPass().get())->GetNative();
		D_CORE_ASSERT(ImGui_ImplVulkan_Init(&info, renderPass), "Failed to initialize ImGui Vulkan");

		VkCommandBuffer commandBuffer = device.BeginSingleTimeCommands();
		ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
		device.EndSingleTimeCommands(commandBuffer);
		ImGui_ImplVulkan_DestroyFontUploadObjects();
		s_Ready = true;
	}

	void VulkanImGuiHelper::Shutdown()
	{
		if (!s_Ready && s_Pool == VK_NULL_HANDLE)
			return;

		VulkanRenderDevice& device = static_cast<VulkanRenderDevice&>(Application::Get().GetDevice());
		if (s_Ready)
			ImGui_ImplVulkan_Shutdown();
		s_Ready = false;

		if (s_Sampler != VK_NULL_HANDLE)
			vkDestroySampler(*device.Device(), s_Sampler, nullptr);
		if (s_Pool != VK_NULL_HANDLE)
			vkDestroyDescriptorPool(*device.Device(), s_Pool, nullptr);
		s_Sampler = VK_NULL_HANDLE;
		s_Pool = VK_NULL_HANDLE;
	}

	void VulkanImGuiHelper::NewFrame()
	{
		if (s_Ready)
			ImGui_ImplVulkan_NewFrame();
	}

	void VulkanImGuiHelper::Render()
	{
		if (!s_Ready)
			return;

		VkCommandBuffer commandBuffer = static_cast<VulkanCommandBuffer&>(Renderer::GetCommandBuffer()).GetNative();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
	}

	void VulkanImGuiHelper::OnResize(uint32_t imageCount)
	{
		if (!s_Ready)
			return;

		uint32_t count = imageCount < 2 ? 2 : imageCount;
		ImGui_ImplVulkan_SetMinImageCount(count);
	}

	void* VulkanImGuiHelper::RegisterTexture(void* imageView, void* sampler)
	{
		if (!s_Ready || imageView == nullptr || s_Sampler == VK_NULL_HANDLE)
			return nullptr;

		VkSampler nativeSampler = sampler != nullptr ? static_cast<VkSampler>(sampler) : s_Sampler;
		VkDescriptorSet set = ImGui_ImplVulkan_AddTexture(nativeSampler, static_cast<VkImageView>(imageView), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		return (void*)set;
	}
}
