#include "dpch.h"
#include "VulkanCommandBuffer.h"

#include "DEngine/Platform/Vulkan/VulkanCommandPool.h"
#include "DEngine/Platform/Vulkan/VulkanFramebuffer.h"
#include "DEngine/Platform/Vulkan/VulkanHelper.h"
#include "DEngine/Platform/Vulkan/VulkanRenderPass.h"

namespace DEngine
{
	VulkanCommandBuffer::VulkanCommandBuffer(RenderDevice& device, VkCommandPool pool, const CommandBufferState& state, const std::shared_ptr<VulkanCommandPoolStatus>& poolStatus)
		: CommandBuffer(device, state)
		, m_Device(static_cast<VulkanRenderDevice&>(device))
		, m_Pool(pool)
		, m_PoolStatus(poolStatus)
	{
		if (m_Pool == VK_NULL_HANDLE)
			return;

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VulkanHelper::CommandBufferLevel(state.level);
		allocInfo.commandPool = m_Pool;
		allocInfo.commandBufferCount = 1;

		D_CORE_ASSERT(vkAllocateCommandBuffers(*m_Device.Device(), &allocInfo, &m_CommandBuffer) == VK_SUCCESS, "Failed to allocate memory for command buffers");
	}

	VulkanCommandBuffer::~VulkanCommandBuffer()
	{
		if (m_CommandBuffer == VK_NULL_HANDLE)
			return;

		VkDevice device = *m_Device.Device();
		if (device == VK_NULL_HANDLE || m_Pool == VK_NULL_HANDLE)
			return;
		if (m_PoolStatus && !m_PoolStatus->alive)
			return;

		vkDeviceWaitIdle(device);
		vkFreeCommandBuffers(device, m_Pool, 1, &m_CommandBuffer);
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

	void VulkanCommandBuffer::BeginSecondary(const CommandBufferInheritance& inheritance)
	{
		VkCommandBufferInheritanceInfo inheritInfo{};
		inheritInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
		inheritInfo.subpass = inheritance.subpass;
		if (inheritance.renderPass)
			inheritInfo.renderPass = static_cast<VulkanRenderPass*>(inheritance.renderPass)->GetNative();
		if (inheritance.framebuffer)
			inheritInfo.framebuffer = static_cast<VulkanFramebuffer*>(inheritance.framebuffer)->GetNative();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
		if (m_State.oneTimeSubmit)
			beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		beginInfo.pInheritanceInfo = &inheritInfo;

		D_CORE_ASSERT(vkBeginCommandBuffer(m_CommandBuffer, &beginInfo) == VK_SUCCESS, "Failed to begin secondary command buffer");
	}

	void VulkanCommandBuffer::End()
	{
		D_CORE_ASSERT(vkEndCommandBuffer(m_CommandBuffer) == VK_SUCCESS, "Failed to record command buffer");
	}

	void VulkanCommandBuffer::ExecuteCommands(const std::vector<Ref<CommandBuffer>>& commands)
	{
		std::vector<VkCommandBuffer> native;
		native.reserve(commands.size());
		for (const Ref<CommandBuffer>& command : commands)
		{
			if (!command || command->GetState().level != DCommandBufferLevel::Secondary)
				continue;

			native.push_back(static_cast<VulkanCommandBuffer*>(command.get())->GetNative());
		}

		if (!native.empty())
			vkCmdExecuteCommands(m_CommandBuffer, static_cast<uint32_t>(native.size()), native.data());
	}

	void VulkanCommandBuffer::Release()
	{
		m_CommandBuffer = VK_NULL_HANDLE;
		m_Pool = VK_NULL_HANDLE;
	}
}
