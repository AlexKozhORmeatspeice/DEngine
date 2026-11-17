#pragma once

#include "DEngine/Core.h"
#include "DEngine/Renderer/Buffers/Buffer.h"
#include "DEngine/Platform/Vulkan/VulkanRendererAPI.h"

#include "stdint.h"
#include "vector"

namespace DEngine
{
	class D_API VulkanVertexBuffer : public VertexBuffer
	{
	public:
		VulkanVertexBuffer(float* verts, uint32_t size, const BufferLayout& layout);
		virtual ~VulkanVertexBuffer();

		virtual void SetLayout(const BufferLayout& layout) override;
		virtual const BufferLayout& GetLayout() const override { return m_Layout; }

		virtual void Bind(CommandBuffer& commandBuffer) const override;
		virtual void Unbind(CommandBuffer& commandBuffer) const override;

		VkBuffer GetNative() const { return m_Allocation.Buffer; }
		const std::vector<VkVertexInputBindingDescription>& GetBindings() const { return m_Bindings; }
		const std::vector<VkVertexInputAttributeDescription>& GetAttributes() const { return m_Attributes; }
	private:
		VulkanAllocatedBuffer m_Allocation;
		BufferLayout m_Layout;
		std::vector<VkVertexInputBindingDescription> m_Bindings;
		std::vector<VkVertexInputAttributeDescription> m_Attributes;
	};

	class D_API VulkanIndexBuffer : public IndexBuffer
	{
	public:
		VulkanIndexBuffer(uint32_t* inds, uint32_t count);
		virtual ~VulkanIndexBuffer();

		virtual uint32_t GetCount() const override { return m_Count; }

		virtual void Bind(CommandBuffer& commandBuffer) const override;
		virtual void Unbind(CommandBuffer& commandBuffer) const override;

		VkBuffer GetNative() const { return m_Allocation.Buffer; }
	private:
		VulkanAllocatedBuffer m_Allocation;
		uint32_t m_Count;
	};
}
