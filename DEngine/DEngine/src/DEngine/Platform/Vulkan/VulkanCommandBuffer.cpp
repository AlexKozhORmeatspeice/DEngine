#include "dpch.h"
#include "VulkanCommandBuffer.h"
#include "DEngine/Platform/Vulkan/VulkanHelper.h"

namespace DEngine
{
	VulkanCommandBuffer::VulkanCommandBuffer(RenderDevice& device, const CommandBufferState& state)
		: CommandBuffer(device, state)
		, m_Device(static_cast<VulkanRenderDevice&>(device))
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VulkanHelper::CommandBufferLevel(state.level);
		allocInfo.commandPool = m_Device.GetCommandPool();
		allocInfo.commandBufferCount = 1;

		D_CORE_ASSERT(vkAllocateCommandBuffers(*m_Device.Device(), &allocInfo, &m_CommandBuffer) == VK_SUCCESS, "Failed to allocate memory for command buffers");
	}

	VulkanCommandBuffer::~VulkanCommandBuffer()
	{
		if (m_CommandBuffer == VK_NULL_HANDLE)
			return;

		VkDevice device = *m_Device.Device();
		VkCommandPool pool = m_Device.GetCommandPool();
		if (device == VK_NULL_HANDLE || pool == VK_NULL_HANDLE)
			return;

		vkDeviceWaitIdle(device);
		vkFreeCommandBuffers(device, pool, 1, &m_CommandBuffer);
		m_CommandBuffer = VK_NULL_HANDLE;
	}

	void VulkanCommandBuffer::Reset()
	{
		if (m_CommandBuffer != VK_NULL_HANDLE)
			vkResetCommandBuffer(m_CommandBuffer, 0);
	}

	void VulkanCommandBuffer::Begin()
	{
		const VkCommandBufferBeginInfo beginInfo = VulkanHelper::CommandBufferBegin(m_State);
		D_CORE_ASSERT(vkBeginCommandBuffer(m_CommandBuffer, &beginInfo) == VK_SUCCESS, "Failed to begin recording command buffer");
	}

	void VulkanCommandBuffer::End()
	{
		D_CORE_ASSERT(vkEndCommandBuffer(m_CommandBuffer) == VK_SUCCESS, "Failed to record command buffer");
	}
}
