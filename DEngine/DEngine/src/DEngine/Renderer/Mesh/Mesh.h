#pragma once

#include "DEngine/Core.h"
#include "DEngine/Renderer/Buffers/VertexArray.h"
#include "DEngine/Renderer/Buffers/Buffer.h"

#include "DEngine/Asset/Asset.h"

namespace DEngine
{
	struct MeshData
	{
		MeshData() = default;
		MeshData(const MeshData& other)
		{
			verts = other.verts;
			vertSize = other.vertSize;
			inds = other.inds;
			indsSize = other.indsSize;
		}

		float* verts = nullptr;
		uint32_t vertSize = 0;
		uint32_t* inds = nullptr;
		uint32_t indsSize = 0;

		void Release()
		{
			delete[] verts;
			delete[] inds;
			verts = nullptr;
			inds = nullptr;
			vertSize = 0;
			indsSize = 0;
		}
	};

	class Mesh : public Asset
	{
	public:
		Mesh(const BufferLayout& layout, const MeshData& data);

		void Bind();
		void Unbind();

		Ref<VertexArray> GetVertexArray() const { return m_VertexArray; }

		static AssetType GetStaticType() { return AssetType::Mesh; }
		virtual AssetType GetType() const override { return GetStaticType(); }

		const BufferLayout& GetLayout() const { return m_Layout; }

	private:
		const BufferLayout m_Layout;

		Ref<VertexArray> m_VertexArray;
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;
	};
}
