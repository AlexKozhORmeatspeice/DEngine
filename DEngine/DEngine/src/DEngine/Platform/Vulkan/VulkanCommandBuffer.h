#pragma once

#include "DEngine/Renderer/CommandBuffer.h"
#include "DEngine/Platform/Vulkan/VulkanRenderDevice.h"

namespace DEngine
{
	class D_API VulkanCommandBuffer : public CommandBuffer
	{
	public:
		explicit VulkanCommandBuffer(RenderDevice& device, VkCommandPool pool, const CommandBufferState& state, const std::shared_ptr<struct VulkanCommandPoolStatus>& poolStatus = {});
		~VulkanCommandBuffer() override;

		void Reset() override;
		void Begin() override;
		void BeginSecondary(const CommandBufferInheritance& inheritance) override;
		void End() override;
		void ExecuteCommands(const std::vector<Ref<CommandBuffer>>& commands) override;
		void Release() override;

		VkCommandBuffer GetNative() const { return m_CommandBuffer; }

	private:
		VulkanRenderDevice& m_Device;
		VkCommandPool m_Pool = VK_NULL_HANDLE;
		VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE;
		std::shared_ptr<struct VulkanCommandPoolStatus> m_PoolStatus;
	};
}
