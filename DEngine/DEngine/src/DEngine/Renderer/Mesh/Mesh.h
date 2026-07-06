#pragma once

#include "DEngine/Core.h"
#include "DEngine/Renderer/Buffers/VertexArray.h"
#include "DEngine/Renderer/Buffers/Buffer.h"

namespace DEngine
{
	//TODO: дописать имплементацию
	class Mesh
	{
	public:
		Mesh(BufferLayout layout, float* verts, uint32_t vertsSize, uint32_t* inds, uint32_t indsSize);

		void Bind();
		void Unbind();

		Ref<VertexArray> GetVertexArray() const { return m_VertexArray; }
	private:
		Ref<VertexArray> m_VertexArray;
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;
	};
}
