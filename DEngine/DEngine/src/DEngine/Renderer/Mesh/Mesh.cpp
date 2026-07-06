#include "dpch.h"
#include "Mesh.h"

namespace DEngine
{
	Mesh::Mesh(BufferLayout layout, float* verts, uint32_t vertsSize, uint32_t* inds, uint32_t indsSize)
	{
		m_VertexArray = VertexArray::Create();
		m_VertexBuffer = VertexBuffer::Create(verts, vertsSize);
		m_VertexBuffer->SetLayout(layout);

		//TODO: добавить проверку на соответствие layout

		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		m_IndexBuffer = IndexBuffer::Create(inds, indsSize);
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