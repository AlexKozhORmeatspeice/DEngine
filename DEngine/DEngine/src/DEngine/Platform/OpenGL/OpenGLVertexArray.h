#pragma once

#include "DEngine/Core.h"
#include "DEngine/Renderer/Buffers/VertexArray.h"

namespace DEngine
{
	class D_API OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void AddVertexBuffer(const DEngine::Ref<VertexBuffer>& vertBuffer) override;
		virtual void SetIndexBuffer(const DEngine::Ref<IndexBuffer>& vertBuffer) override;

		virtual const std::vector<DEngine::Ref<VertexBuffer>>& GetVertexBuffers() const override { return m_VertexBuffers; }
		virtual const DEngine::Ref<IndexBuffer>& GetIndexBuffer() const override { return m_IndexBuffer; }
	private:
		std::vector<DEngine::Ref<VertexBuffer>> m_VertexBuffers;
		DEngine::Ref<IndexBuffer> m_IndexBuffer;

		uint32_t m_RendererID;
	};
}
