#pragma once
#include "DEngine/Core.h"

#include "DEngine/Renderer/RendererAPI.h"

#include "vk_mem_alloc.h"
#include "DEngine/Platform/Vulkan/VulkanCommandBuffer.h"

#include "unordered_map"
#include "vector"

namespace DEngine
{
	struct VulkanAllocatedBuffer
	{
		VkBuffer Buffer = VK_NULL_HANDLE;
		VmaAllocation Allocation = VK_NULL_HANDLE;
	};

	struct VulkanAllocatedImage
	{
		VkImage Image = VK_NULL_HANDLE;
		VmaAllocation Allocation = VK_NULL_HANDLE;
	};

	class D_API VulkanRendererAPI : public RendererAPI
	{
	public:
		VulkanRendererAPI();
		~VulkanRendererAPI();

		static VulkanRendererAPI& Get();
		static VulkanRendererAPI* TryGet() { return s_Instance; }

		virtual void Init() override;
		virtual void Shutdown() override;
		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;

		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		virtual void DrawIndexed(CommandBuffer& commandBuffer, const DEngine::Ref<VertexArray>& vertArr) override;
		virtual void DrawIndexedWireframe(CommandBuffer& commandBuffer, const DEngine::Ref<VertexArray>& vertArr) override;

		VulkanAllocatedBuffer AllocateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage, VmaAllocationCreateFlags allocationFlags = 0);
		void FreeBuffer(VulkanAllocatedBuffer& buffer);
		void UploadBuffer(const VulkanAllocatedBuffer& destination, const void* data, VkDeviceSize size);

		VmaAllocator GetAllocator() const { return m_Allocator; }

		VkDescriptorSet AllocateDescriptorSet(VkDescriptorSetLayout layout);
		void FreeDescriptorSet(VkDescriptorSet set);
		void UpdateDescriptorSetUniform(VkDescriptorSet set, uint32_t binding, VkBuffer buffer, VkDeviceSize size);
		void UpdateDescriptorSetTexture(VkDescriptorSet set, uint32_t binding, VkImageView imageView, VkSampler sampler);

		VulkanAllocatedImage AllocateImage(const VkImageCreateInfo& imageInfo);
		void FreeImage(VulkanAllocatedImage& image);

	private:
		VkDescriptorPool CreateDescriptorPool();

		static VulkanRendererAPI* s_Instance;
		VmaAllocator m_Allocator = VK_NULL_HANDLE;

		std::vector<VkDescriptorPool> m_DescriptorPools;
		std::unordered_map<VkDescriptorSet, VkDescriptorPool> m_DescriptorSetPools;
	};
}
