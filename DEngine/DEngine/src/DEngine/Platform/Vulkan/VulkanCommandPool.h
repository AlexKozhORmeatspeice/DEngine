#pragma once

#include "DEngine/Renderer/CommandPool.h"
#include "DEngine/Platform/Vulkan/VulkanRenderDevice.h"

#include "memory"

namespace DEngine
{
	struct VulkanCommandPoolStatus
	{
		bool alive = true;
	};

	class D_API VulkanCommandPool : public CommandPool
	{
	public:
		VulkanCommandPool(RenderDevice& device, const CommandPoolState& state);
		~VulkanCommandPool() override;

		void Reset() override;

		VkCommandPool GetNative() const { return m_Pool; }

	protected:
		Ref<CommandBuffer> AllocateNative(const CommandBufferState& state) override;

	private:
		VulkanRenderDevice& m_Device;
		VkCommandPool m_Pool = VK_NULL_HANDLE;
		std::shared_ptr<VulkanCommandPoolStatus> m_Status;
	};
}
