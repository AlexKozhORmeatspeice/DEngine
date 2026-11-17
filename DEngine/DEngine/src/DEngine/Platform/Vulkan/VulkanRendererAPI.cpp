#include "dpch.h"
#include "VulkanRendererAPI.h"

#include "DEngine/Application.h"
#include "DEngine/Platform/Vulkan/VulkanRenderDevice.h"

#include "cstring"

namespace DEngine
{
	VulkanRendererAPI* VulkanRendererAPI::s_Instance = nullptr;

	VulkanRendererAPI::VulkanRendererAPI()
	{
		s_Instance = this;
	}

	VulkanRendererAPI::~VulkanRendererAPI()
	{
		m_Allocator = VK_NULL_HANDLE;
		m_DescriptorPools.clear();
		m_DescriptorSetPools.clear();
		if (s_Instance == this)
			s_Instance = nullptr;
	}

	VulkanRendererAPI& VulkanRendererAPI::Get()
	{
		D_CORE_ASSERT(s_Instance, "Vulkan renderer API is not created");
		return *s_Instance;
	}

	void VulkanRendererAPI::Init()
	{
		if (m_Allocator != VK_NULL_HANDLE)
			return;

		VulkanRenderDevice& device = static_cast<VulkanRenderDevice&>(Application::Get().GetDevice());

		VmaAllocatorCreateInfo createInfo{};
		createInfo.instance = device.GetInstance();
		createInfo.physicalDevice = device.GetPhysicalDevice();
		createInfo.device = *device.Device();
		createInfo.vulkanApiVersion = VK_API_VERSION_1_0;

		D_CORE_ASSERT(vmaCreateAllocator(&createInfo, &m_Allocator) == VK_SUCCESS, "Failed to create Vulkan memory allocator");
		CreateDescriptorPool();
	}

	void VulkanRendererAPI::Shutdown()
	{
		VulkanRenderDevice& device = static_cast<VulkanRenderDevice&>(Application::Get().GetDevice());
		for (VkDescriptorPool pool : m_DescriptorPools)
		{
			if (pool != VK_NULL_HANDLE)
				vkDestroyDescriptorPool(*device.Device(), pool, nullptr);
		}
		m_DescriptorPools.clear();
		m_DescriptorSetPools.clear();

		if (m_Allocator != VK_NULL_HANDLE)
		{
			vmaDestroyAllocator(m_Allocator);
			m_Allocator = VK_NULL_HANDLE;
		}
	}

	void VulkanRendererAPI::SetClearColor(const glm::vec4& color)
	{
	}

	void VulkanRendererAPI::Clear()
	{
	}

	void VulkanRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
	}

	void VulkanRendererAPI::DrawIndexed(CommandBuffer& commandBuffer, const DEngine::Ref<VertexArray>& vertArr)
	{
		if (!vertArr || !vertArr->GetIndexBuffer())
			return;

		vertArr->Bind(commandBuffer);
		vkCmdDrawIndexed(static_cast<VulkanCommandBuffer&>(commandBuffer).GetNative(), vertArr->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
	}

	void VulkanRendererAPI::DrawIndexedWireframe(CommandBuffer& commandBuffer, const DEngine::Ref<VertexArray>& vertArr)
	{
		//TODO: нужно создать отдельную PSO для Wireframe отрисовки
		DrawIndexed(commandBuffer, vertArr);
	}

	VulkanAllocatedBuffer VulkanRendererAPI::AllocateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage, VmaAllocationCreateFlags allocationFlags)
	{
		VulkanAllocatedBuffer result;
		if (size == 0 || m_Allocator == VK_NULL_HANDLE)
			return result;

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocationInfo{};
		allocationInfo.usage = memoryUsage;
		allocationInfo.flags = allocationFlags;

		D_CORE_ASSERT(vmaCreateBuffer(m_Allocator, &bufferInfo, &allocationInfo, &result.Buffer, &result.Allocation, nullptr) == VK_SUCCESS, "Failed to allocate buffer");

		if (allocationFlags & (VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT))
		{
			VkMemoryPropertyFlags properties = 0;
			vmaGetAllocationMemoryProperties(m_Allocator, result.Allocation, &properties);
			D_CORE_ASSERT(properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, "Host-access buffer is not host visible");
		}

		return result;
	}

	void VulkanRendererAPI::FreeBuffer(VulkanAllocatedBuffer& buffer)
	{
		if (m_Allocator == VK_NULL_HANDLE || buffer.Buffer == VK_NULL_HANDLE)
			return;

		vmaDestroyBuffer(m_Allocator, buffer.Buffer, buffer.Allocation);
		buffer.Buffer = VK_NULL_HANDLE;
		buffer.Allocation = VK_NULL_HANDLE;
	}

	void VulkanRendererAPI::UploadBuffer(const VulkanAllocatedBuffer& destination, const void* data, VkDeviceSize size)
	{
		if (!data || size == 0 || destination.Buffer == VK_NULL_HANDLE)
			return;

		VulkanAllocatedBuffer staging = AllocateBuffer(
			size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VMA_MEMORY_USAGE_AUTO,
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);

		void* mapped = nullptr;
		D_CORE_ASSERT(vmaMapMemory(m_Allocator, staging.Allocation, &mapped) == VK_SUCCESS, "Failed to map staging buffer");
		memcpy(mapped, data, static_cast<size_t>(size));
		vmaFlushAllocation(m_Allocator, staging.Allocation, 0, size);
		vmaUnmapMemory(m_Allocator, staging.Allocation);

		VulkanRenderDevice& device = static_cast<VulkanRenderDevice&>(Application::Get().GetDevice());
		device.CopyBuffer(staging.Buffer, destination.Buffer, size);
		FreeBuffer(staging);
	}

	VkDescriptorPool VulkanRendererAPI::CreateDescriptorPool()
	{
		VulkanRenderDevice& device = static_cast<VulkanRenderDevice&>(Application::Get().GetDevice());

		VkDescriptorPoolSize poolSizes[2]{};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = 1024;
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = 1024;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		poolInfo.maxSets = 1024;
		poolInfo.poolSizeCount = 2;
		poolInfo.pPoolSizes = poolSizes;

		VkDescriptorPool pool = VK_NULL_HANDLE;
		D_CORE_ASSERT(vkCreateDescriptorPool(*device.Device(), &poolInfo, nullptr, &pool) == VK_SUCCESS, "Failed to create descriptor pool");
		m_DescriptorPools.push_back(pool);
		return pool;
	}

	VkDescriptorSet VulkanRendererAPI::AllocateDescriptorSet(VkDescriptorSetLayout layout)
	{
		if (layout == VK_NULL_HANDLE)
			return VK_NULL_HANDLE;

		if (m_DescriptorPools.empty())
			CreateDescriptorPool();

		VulkanRenderDevice& device = static_cast<VulkanRenderDevice&>(Application::Get().GetDevice());
		VkDescriptorSetAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocateInfo.descriptorPool = m_DescriptorPools.back();
		allocateInfo.descriptorSetCount = 1;
		allocateInfo.pSetLayouts = &layout;

		VkDescriptorSet set = VK_NULL_HANDLE;
		VkResult result = vkAllocateDescriptorSets(*device.Device(), &allocateInfo, &set);
		if (result == VK_ERROR_OUT_OF_POOL_MEMORY || result == VK_ERROR_FRAGMENTED_POOL)
		{
			allocateInfo.descriptorPool = CreateDescriptorPool();
			result = vkAllocateDescriptorSets(*device.Device(), &allocateInfo, &set);
		}

		D_CORE_ASSERT(result == VK_SUCCESS, "Failed to allocate descriptor set");
		m_DescriptorSetPools.emplace(set, allocateInfo.descriptorPool);
		return set;
	}

	void VulkanRendererAPI::FreeDescriptorSet(VkDescriptorSet set)
	{
		if (set == VK_NULL_HANDLE)
			return;

		auto pool = m_DescriptorSetPools.find(set);
		if (pool == m_DescriptorSetPools.end() || pool->second == VK_NULL_HANDLE)
			return;

		VulkanRenderDevice& device = static_cast<VulkanRenderDevice&>(Application::Get().GetDevice());
		vkFreeDescriptorSets(*device.Device(), pool->second, 1, &set);
		m_DescriptorSetPools.erase(pool);
	}

	void VulkanRendererAPI::UpdateDescriptorSetUniform(VkDescriptorSet set, uint32_t binding, VkBuffer buffer, VkDeviceSize size)
	{
		VulkanRenderDevice& device = static_cast<VulkanRenderDevice&>(Application::Get().GetDevice());

		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = buffer;
		bufferInfo.offset = 0;
		bufferInfo.range = size;

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet = set;
		write.dstBinding = binding;
		write.descriptorCount = 1;
		write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		write.pBufferInfo = &bufferInfo;
		vkUpdateDescriptorSets(*device.Device(), 1, &write, 0, nullptr);
	}

	void VulkanRendererAPI::UpdateDescriptorSetTexture(VkDescriptorSet set, uint32_t binding, VkImageView imageView, VkSampler sampler)
	{
		if (imageView == VK_NULL_HANDLE || sampler == VK_NULL_HANDLE)
			return;

		VulkanRenderDevice& device = static_cast<VulkanRenderDevice&>(Application::Get().GetDevice());

		VkDescriptorImageInfo imageInfo{};
		imageInfo.sampler = sampler;
		imageInfo.imageView = imageView;
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet = set;
		write.dstBinding = binding;
		write.descriptorCount = 1;
		write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		write.pImageInfo = &imageInfo;
		vkUpdateDescriptorSets(*device.Device(), 1, &write, 0, nullptr);
	}

	VulkanAllocatedImage VulkanRendererAPI::AllocateImage(const VkImageCreateInfo& imageInfo)
	{
		VulkanAllocatedImage result;
		if (m_Allocator == VK_NULL_HANDLE)
			return result;

		VmaAllocationCreateInfo allocationInfo{};
		allocationInfo.usage = VMA_MEMORY_USAGE_AUTO;

		D_CORE_ASSERT(vmaCreateImage(m_Allocator, &imageInfo, &allocationInfo, &result.Image, &result.Allocation, nullptr) == VK_SUCCESS, "Failed to allocate image");
		return result;
	}

	void VulkanRendererAPI::FreeImage(VulkanAllocatedImage& image)
	{
		if (m_Allocator == VK_NULL_HANDLE || image.Image == VK_NULL_HANDLE)
			return;

		vmaDestroyImage(m_Allocator, image.Image, image.Allocation);
		image.Image = VK_NULL_HANDLE;
		image.Allocation = VK_NULL_HANDLE;
	}
}
