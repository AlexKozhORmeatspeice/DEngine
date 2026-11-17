#pragma once

#include "DEngine/Renderer/Pipeline/PipelineLayout.h"
#include "DEngine/Platform/Vulkan/VulkanRenderDevice.h"

#include "vector"

namespace DEngine
{
	class D_API VulkanPipelineLayout : public PipelineLayout
	{
	public:
		explicit VulkanPipelineLayout(RenderDevice& device, const PipelineLayoutState& state);
		~VulkanPipelineLayout() override;

		VkPipelineLayout GetNative() const { return m_Layout; }
		VkDescriptorSetLayout GetSetLayout(uint32_t index) const { return m_SetLayouts[index]; }
		void PushConstant(CommandBuffer& commandBuffer, uint32_t stageFlags, uint32_t offset, uint32_t size, const void* data) override;

	private:
		VulkanRenderDevice& m_Device;
		VkPipelineLayout m_Layout = VK_NULL_HANDLE;
		std::vector<VkDescriptorSetLayout> m_SetLayouts;
	};
}
