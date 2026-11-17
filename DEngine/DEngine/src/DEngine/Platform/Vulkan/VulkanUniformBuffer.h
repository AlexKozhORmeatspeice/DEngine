#pragma once

#include "DEngine/Renderer/Buffers/UniformBuffer.h"
#include "DEngine/Platform/Vulkan/VulkanRendererAPI.h"

namespace DEngine
{
	class D_API VulkanUniformBuffer : public UniformBuffer
	{
	public:
		explicit VulkanUniformBuffer(uint32_t size);
		~VulkanUniformBuffer() override;

		void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;
		const void* GetData() const override { return m_Data.data(); }
		uint32_t GetSize() const override { return static_cast<uint32_t>(m_Data.size()); }

		VkBuffer GetNative() const { return m_Allocation.Buffer; }

	private:
		VulkanAllocatedBuffer m_Allocation;
		std::vector<uint8_t> m_Data;
	};
}
