#include "dpch.h"
#include "VulkanVertexArray.h"

namespace DEngine
{
	VulkanVertexArray::VulkanVertexArray()
	{
	}

	VulkanVertexArray::~VulkanVertexArray()
	{
	}

	void VulkanVertexArray::Bind() const
	{
	}

	void VulkanVertexArray::Unbind() const
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
