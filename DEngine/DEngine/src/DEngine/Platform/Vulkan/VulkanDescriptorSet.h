#pragma once

#include "DEngine/Renderer/Pipeline/DescriptorSet.h"
#include "vulkan/vulkan.h"

namespace DEngine
{
	class D_API VulkanDescriptorSet : public DescriptorSet
	{
	public:
		VulkanDescriptorSet(const Ref<PipelineLayout>& layout, uint32_t setIndex);
		~VulkanDescriptorSet() override;

		void WriteUniform(uint32_t binding, const Ref<UniformBuffer>& buffer) override;
		void WriteTexture(uint32_t binding, const Ref<Texture>& texture) override;
		void Bind(CommandBuffer& commandBuffer, const Ref<PipelineLayout>& layout) override;

		VkDescriptorSet GetNative() const { return m_Set; }

	private:
		uint32_t m_SetIndex = 0;
		VkDescriptorSet m_Set = VK_NULL_HANDLE;
	};
}
