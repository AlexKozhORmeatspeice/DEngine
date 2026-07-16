#include "dpch.h"
#include "Mesh.h"

namespace DEngine
{
	Mesh::Mesh(const BufferLayout& layout, const MeshData& data)
		: m_Layout(layout)
	{
		m_VertexArray = VertexArray::Create();
		m_VertexBuffer = VertexBuffer::Create(data.verts, data.vertSize);
		m_VertexBuffer->SetLayout(layout);

		//TODO: добавить проверку на соответствие layout

		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		m_IndexBuffer = IndexBuffer::Create(data.inds, data.indsSize);
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);
	}

	void Mesh::Bind()
	{
		m_VertexArray->Bind();
	}

	void Mesh::Unbind()
	{
		m_VertexArray->Unbind();
	}
}