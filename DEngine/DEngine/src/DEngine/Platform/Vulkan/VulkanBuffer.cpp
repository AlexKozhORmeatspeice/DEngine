#include "dpch.h"
#include "VulkanBuffer.h"
#include "DEngine/Platform/Vulkan/VulkanHelper.h"
#include "DEngine/Platform/Vulkan/VulkanCommandBuffer.h"

namespace DEngine
{
	VulkanVertexBuffer::VulkanVertexBuffer(float* verts, uint32_t size, const BufferLayout& layout)
	{
		if (size > 0)
		{
			m_Allocation = VulkanRendererAPI::Get().AllocateBuffer(
				size,
				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				VMA_MEMORY_USAGE_AUTO);
			VulkanRendererAPI::Get().UploadBuffer(m_Allocation, verts, size);
		}

		SetLayout(layout);
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		if (VulkanRendererAPI* api = VulkanRendererAPI::TryGet())
			api->FreeBuffer(m_Allocation);
	}

	void VulkanVertexBuffer::SetLayout(const BufferLayout& layout)
	{
		m_Layout = layout;
		m_Bindings.clear();
		m_Attributes.clear();

		if (layout.GetElements().empty())
			return;

		m_Bindings.push_back(VulkanHelper::VertexBinding(layout));
		m_Attributes = VulkanHelper::VertexAttributes(layout);
	}

	void VulkanVertexBuffer::Bind(CommandBuffer& commandBuffer) const
	{
		if (m_Allocation.Buffer == VK_NULL_HANDLE)
			return;

		VkBuffer buffers[] = { m_Allocation.Buffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(static_cast<VulkanCommandBuffer&>(commandBuffer).GetNative(), 0, 1, buffers, offsets);
	}

	void VulkanVertexBuffer::Unbind(CommandBuffer& commandBuffer) const
	{
	}

	VulkanIndexBuffer::VulkanIndexBuffer(uint32_t* inds, uint32_t count)
		: m_Count(count)
	{
		const VkDeviceSize size = static_cast<VkDeviceSize>(count) * sizeof(uint32_t);
		if (size == 0)
			return;

		m_Allocation = VulkanRendererAPI::Get().AllocateBuffer(
			size,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VMA_MEMORY_USAGE_AUTO);
		
		VulkanRendererAPI::Get().UploadBuffer(m_Allocation, inds, size);
	}

	VulkanIndexBuffer::~VulkanIndexBuffer()
	{
		if (VulkanRendererAPI* api = VulkanRendererAPI::TryGet())
			api->FreeBuffer(m_Allocation);
	}

	void VulkanIndexBuffer::Bind(CommandBuffer& commandBuffer) const
	{
		if (m_Allocation.Buffer == VK_NULL_HANDLE)
			return;

		vkCmdBindIndexBuffer(static_cast<VulkanCommandBuffer&>(commandBuffer).GetNative(), m_Allocation.Buffer, 0, VK_INDEX_TYPE_UINT32);
	}

	void VulkanIndexBuffer::Unbind(CommandBuffer& commandBuffer) const
	{
	}
}
