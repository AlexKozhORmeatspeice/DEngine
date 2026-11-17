#include "dpch.h"
#include "VulkanVertexArray.h"
#include "DEngine/Platform/Vulkan/VulkanBuffer.h"
#include "DEngine/Platform/Vulkan/VulkanCommandBuffer.h"

namespace DEngine
{
	VulkanVertexArray::VulkanVertexArray()
	{
	}

	VulkanVertexArray::~VulkanVertexArray()
	{
	}

	void VulkanVertexArray::Bind(CommandBuffer& commandBuffer) const
	{
		for (const Ref<VertexBuffer>& buffer : m_VertexBuffers)
		{
			if (buffer)
				buffer->Bind(commandBuffer);
		}

		if (m_IndexBuffer)
			m_IndexBuffer->Bind(commandBuffer);
	}

	void VulkanVertexArray::Unbind(CommandBuffer& commandBuffer) const
	{
	}

	void VulkanVertexArray::AddVertexBuffer(const DEngine::Ref<VertexBuffer>& vertBuffer)
	{
		if (!vertBuffer)
			return;

		m_VertexBuffers.push_back(vertBuffer);
	}

	void VulkanVertexArray::SetIndexBuffer(const DEngine::Ref<IndexBuffer>& vertBuffer)
	{
		if (!vertBuffer)
			return;

		m_IndexBuffer = vertBuffer;
	}
}
