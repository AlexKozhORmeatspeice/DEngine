#include "dpch.h"
#include "Mesh.h"

#include "DEngine/Profiler/Profiler.h"

namespace DEngine
{
	Mesh::Mesh(const BufferLayout& layout, const MeshData& data)
		: m_Layout(layout)
	{
		m_VertexArray = VertexArray::Create();
		m_VertexBuffer = VertexBuffer::Create(data.verts, data.vertSize, layout);
		m_IndexBuffer = IndexBuffer::Create(data.inds, data.indsSize);
		if (!m_VertexArray || !m_VertexBuffer || !m_IndexBuffer)
			return;

		m_VertexBuffer->SetLayout(layout);

		//TODO: добавить проверку на соответствие layout
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);
	}

	void Mesh::Bind(CommandBuffer& commandBuffer)
	{
		D_PROFILE_FUNCTION();

		if (m_VertexArray)
			m_VertexArray->Bind(commandBuffer);
	}

	void Mesh::Unbind(CommandBuffer& commandBuffer)
	{
		if (m_VertexArray)
			m_VertexArray->Unbind(commandBuffer);
	}
}