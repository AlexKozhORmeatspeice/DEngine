#pragma once

#include "DEngine/Core.h"
#include "DEngine/Renderer/Buffers/VertexArray.h"
#include "DEngine/Renderer/Buffers/Buffer.h"

#include "DEngine/Asset/Asset.h"

namespace DEngine
{
	class Mesh : public Asset
	{
	public:
		Mesh(const BufferLayout& layout, float* verts, uint32_t vertsSize, uint32_t* inds, uint32_t indsSize);

		void Bind();
		void Unbind();

		Ref<VertexArray> GetVertexArray() const { return m_VertexArray; }

		static AssetType GetStaticType() { return AssetType::Mesh; }
		virtual AssetType GetType() const override { return GetStaticType(); }

	private:
		Ref<VertexArray> m_VertexArray;
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;
	};
}
