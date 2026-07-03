#pragma once

#include "DEngine/Core.h"
#include "DEngine/Renderer/Buffers/Buffer.h"
#include "memory"

namespace DEngine
{
	class D_API VertexArray
	{
	public:
		virtual ~VertexArray() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void AddVertexBuffer(const DEngine::Ref<VertexBuffer>& vertBuffer) = 0;
		virtual void SetIndexBuffer(const DEngine::Ref<IndexBuffer>& vertBuffer) = 0;

		virtual const std::vector<DEngine::Ref<VertexBuffer>>& GetVertexBuffers() const = 0;
		virtual const DEngine::Ref<IndexBuffer>& GetIndexBuffer() const = 0;

		static VertexArray* Create();
	protected:
		VertexArray() {}
	};
}
