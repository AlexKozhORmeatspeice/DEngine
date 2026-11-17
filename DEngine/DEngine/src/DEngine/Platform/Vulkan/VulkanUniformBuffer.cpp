#include "dpch.h"
#include "VulkanUniformBuffer.h"

#include "cstring"

namespace DEngine
{
	VulkanUniformBuffer::VulkanUniformBuffer(uint32_t size)
		: m_Data(size)
	{
		m_Allocation = VulkanRendererAPI::Get().AllocateBuffer(
			size,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VMA_MEMORY_USAGE_AUTO,
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
	}

	VulkanUniformBuffer::~VulkanUniformBuffer()
	{
		if (VulkanRendererAPI::TryGet())
			VulkanRendererAPI::Get().FreeBuffer(m_Allocation);
	}

	void VulkanUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
	{
		if (!data || offset + size > m_Data.size() || m_Allocation.Allocation == VK_NULL_HANDLE)
			return;

		std::memcpy(m_Data.data() + offset, data, size);

		VmaAllocator allocator = VulkanRendererAPI::Get().GetAllocator();
		void* mapped = nullptr;
		D_CORE_ASSERT(vmaMapMemory(allocator, m_Allocation.Allocation, &mapped) == VK_SUCCESS, "Failed to map uniform buffer");
		std::memcpy(static_cast<uint8_t*>(mapped) + offset, data, size);
		vmaFlushAllocation(allocator, m_Allocation.Allocation, offset, size);
		vmaUnmapMemory(allocator, m_Allocation.Allocation);
	}
}
