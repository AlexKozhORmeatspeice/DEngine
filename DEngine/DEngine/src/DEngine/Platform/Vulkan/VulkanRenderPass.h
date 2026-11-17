#pragma once

#include "DEngine/Renderer/Pipeline/RenderPass.h"
#include "DEngine/Platform/Vulkan/VulkanRenderDevice.h"

namespace DEngine
{
	class VulkanSwapChain;

	class D_API VulkanRenderPass : public RenderPass
	{
	public:
		explicit VulkanRenderPass(RenderDevice& device, VulkanSwapChain& swapChain);
		VulkanRenderPass(RenderDevice& device, VkFormat colorFormat, VkFormat depthFormat, VkImageLayout colorFinalLayout);
		~VulkanRenderPass() override;

		VkRenderPass GetNative() const { return m_RenderPass; }

		void Begin(CommandBuffer& commandBuffer, Framebuffer& framebuffer, const RenderPassState& state) override;
		void End(CommandBuffer& commandBuffer) override;

	private:
		VulkanRenderDevice& m_Device;
		VkRenderPass m_RenderPass = VK_NULL_HANDLE;
	};
}
