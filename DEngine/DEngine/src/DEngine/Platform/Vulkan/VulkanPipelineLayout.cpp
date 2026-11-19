#include "dpch.h"
#include "VulkanPipelineLayout.h"
#include "DEngine/Platform/Vulkan/VulkanHelper.h"
#include "DEngine/Platform/Vulkan/VulkanCommandBuffer.h"

#include "DEngine/Profiler/Profiler.h"

namespace DEngine
{
	VulkanPipelineLayout::VulkanPipelineLayout(RenderDevice& device, const PipelineLayoutState& state)
		: PipelineLayout(device, state)
		, m_Device(static_cast<VulkanRenderDevice&>(device))
	{
		m_SetLayouts.reserve(state.setLayouts.size());
		for (const DDescriptorSetLayout& layout : state.setLayouts)
		{
			std::vector<VkDescriptorSetLayoutBinding> bindings;
			bindings.reserve(layout.bindings.size());
			for (const DDescriptorBinding& binding : layout.bindings)
				bindings.push_back(VulkanHelper::DescriptorBinding(binding));

			VkDescriptorSetLayoutCreateInfo setLayoutInfo{};
			setLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			setLayoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
			setLayoutInfo.pBindings = bindings.empty() ? nullptr : bindings.data();

			VkDescriptorSetLayout setLayout = VK_NULL_HANDLE;
			D_CORE_ASSERT(vkCreateDescriptorSetLayout(*m_Device.Device(), &setLayoutInfo, nullptr, &setLayout) == VK_SUCCESS, "Can't create descriptor set layout");
			m_SetLayouts.push_back(setLayout);
		}

		std::vector<VkPushConstantRange> pushConstants;
		pushConstants.reserve(state.pushConstantRanges.size());
		for (const DPushConstantRange& range : state.pushConstantRanges)
			pushConstants.push_back(VulkanHelper::PushConstantRange(range));

		const VkPipelineLayoutCreateInfo pipelineLayoutInfo = VulkanHelper::PipelineLayout(
			state,
			m_SetLayouts.empty() ? nullptr : m_SetLayouts.data(),
			pushConstants.empty() ? nullptr : pushConstants.data());

		D_CORE_ASSERT(vkCreatePipelineLayout(*m_Device.Device(), &pipelineLayoutInfo, nullptr, &m_Layout) == VK_SUCCESS, "Can't create pipeline layout");
	}

	VulkanPipelineLayout::~VulkanPipelineLayout()
	{
		VkDevice device = *m_Device.Device();

		if (m_Layout != VK_NULL_HANDLE)
			vkDestroyPipelineLayout(device, m_Layout, nullptr);

		for (VkDescriptorSetLayout setLayout : m_SetLayouts)
		{
			if (setLayout != VK_NULL_HANDLE)
				vkDestroyDescriptorSetLayout(device, setLayout, nullptr);
		}
	}

	void VulkanPipelineLayout::PushConstant(CommandBuffer& commandBuffer, uint32_t stageFlags, uint32_t offset, uint32_t size, const void* data)
	{
		D_PROFILE_FUNCTION();
		if (!data || size == 0 || m_Layout == VK_NULL_HANDLE)
			return;

		VkShaderStageFlags stages = 0;
		if (stageFlags & DShaderStage::Vertex)
			stages |= VK_SHADER_STAGE_VERTEX_BIT;
		if (stageFlags & DShaderStage::Fragment)
			stages |= VK_SHADER_STAGE_FRAGMENT_BIT;

		vkCmdPushConstants(static_cast<VulkanCommandBuffer&>(commandBuffer).GetNative(), m_Layout, stages, offset, size, data);
	}
}
