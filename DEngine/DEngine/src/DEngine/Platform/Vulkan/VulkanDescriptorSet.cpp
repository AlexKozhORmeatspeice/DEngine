#include "dpch.h"
#include "VulkanDescriptorSet.h"

#include "DEngine/Platform/Vulkan/VulkanCommandBuffer.h"
#include "DEngine/Platform/Vulkan/VulkanPipelineLayout.h"
#include "DEngine/Platform/Vulkan/VulkanRendererAPI.h"
#include "DEngine/Platform/Vulkan/VulkanTexture.h"
#include "DEngine/Platform/Vulkan/VulkanUniformBuffer.h"
#include "DEngine/Renderer/Pipeline/PipelineLayout.h"

namespace DEngine
{
	VulkanDescriptorSet::VulkanDescriptorSet(const Ref<PipelineLayout>& layout, uint32_t setIndex)
		: m_SetIndex(setIndex)
	{
		auto* nativeLayout = static_cast<VulkanPipelineLayout*>(layout.get());
		m_Set = VulkanRendererAPI::Get().AllocateDescriptorSet(nativeLayout->GetSetLayout(setIndex));
	}

	VulkanDescriptorSet::~VulkanDescriptorSet()
	{
		if (VulkanRendererAPI::TryGet() && m_Set != VK_NULL_HANDLE)
			VulkanRendererAPI::Get().FreeDescriptorSet(m_Set);
	}

	void VulkanDescriptorSet::WriteUniform(uint32_t binding, const Ref<UniformBuffer>& buffer)
	{
		if (!buffer || m_Set == VK_NULL_HANDLE)
			return;

		auto* native = static_cast<VulkanUniformBuffer*>(buffer.get());
		VulkanRendererAPI::Get().UpdateDescriptorSetUniform(m_Set, binding, native->GetNative(), buffer->GetSize());
	}

	void VulkanDescriptorSet::WriteTexture(uint32_t binding, const Ref<Texture>& texture)
	{
		if (!texture || m_Set == VK_NULL_HANDLE)
			return;

		auto* native = static_cast<VulkanTexture2D*>(texture.get());
		VulkanRendererAPI::Get().UpdateDescriptorSetTexture(m_Set, binding, native->GetImageView(), native->GetSampler());
	}

	void VulkanDescriptorSet::Bind(CommandBuffer& commandBuffer, const Ref<PipelineLayout>& layout)
	{
		if (m_Set == VK_NULL_HANDLE || !layout)
			return;

		VkDescriptorSet sets[] = { m_Set };
		vkCmdBindDescriptorSets(
			static_cast<VulkanCommandBuffer&>(commandBuffer).GetNative(),
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			static_cast<VulkanPipelineLayout*>(layout.get())->GetNative(),
			m_SetIndex,
			1,
			sets,
			0,
			nullptr);
	}
}
