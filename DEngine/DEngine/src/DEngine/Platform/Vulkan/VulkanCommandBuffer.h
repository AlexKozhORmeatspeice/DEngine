#pragma once

#include "DEngine/Renderer/CommandBuffer.h"
#include "DEngine/Platform/Vulkan/VulkanRenderDevice.h"

namespace DEngine
{
	class D_API VulkanCommandBuffer : public CommandBuffer
	{
	public:
		explicit VulkanCommandBuffer(RenderDevice& device, const CommandBufferState& state);
		~VulkanCommandBuffer() override;

		void Reset() override;
		void Begin() override;
		void End() override;

		VkCommandBuffer GetNative() const { return m_CommandBuffer; }

	private:
		VulkanRenderDevice& m_Device;
		VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE;
	};
}
