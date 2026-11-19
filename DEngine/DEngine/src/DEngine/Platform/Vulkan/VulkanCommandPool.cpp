#include "dpch.h"
#include "VulkanCommandPool.h"

#include "DEngine/Platform/Vulkan/VulkanCommandBuffer.h"

namespace DEngine
{
	VulkanCommandPool::VulkanCommandPool(RenderDevice& device, const CommandPoolState& state)
		: m_Device(static_cast<VulkanRenderDevice&>(device))
		, m_Status(std::make_shared<VulkanCommandPoolStatus>())
	{
		const QueueFamilyIndices indices = m_Device.FindPhysicalQueueFamilies();

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = indices.graphicsFamily;
		if (state.transient)
			poolInfo.flags |= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
		if (state.resetBuffers)
			poolInfo.flags |= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		D_CORE_ASSERT(vkCreateCommandPool(*m_Device.Device(), &poolInfo, nullptr, &m_Pool) == VK_SUCCESS, "Failed to create command pool");
	}

	VulkanCommandPool::~VulkanCommandPool()
	{
		VkDevice device = m_Device.Device() ? *m_Device.Device() : VK_NULL_HANDLE;
		if (device != VK_NULL_HANDLE && m_Pool != VK_NULL_HANDLE)
			vkDeviceWaitIdle(device);

		if (m_Status)
			m_Status->alive = false;

		for (Ref<CommandBuffer>& buffer : m_Buffers)
		{
			if (buffer)
				buffer->Release();
		}
		m_Buffers.clear();

		if (device != VK_NULL_HANDLE && m_Pool != VK_NULL_HANDLE)
			vkDestroyCommandPool(device, m_Pool, nullptr);
		m_Pool = VK_NULL_HANDLE;
	}

	void VulkanCommandPool::Reset()
	{
		if (m_Pool == VK_NULL_HANDLE)
			return;

		D_CORE_ASSERT(vkResetCommandPool(*m_Device.Device(), m_Pool, 0) == VK_SUCCESS, "Failed to reset command pool");
	}

	Ref<CommandBuffer> VulkanCommandPool::AllocateNative(const CommandBufferState& state)
	{
		if (m_Pool == VK_NULL_HANDLE)
			return nullptr;

		return CreateRef<VulkanCommandBuffer>(m_Device, m_Pool, state, m_Status);
	}
}
