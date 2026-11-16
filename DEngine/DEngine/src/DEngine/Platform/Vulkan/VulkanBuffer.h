#pragma once

#include "DEngine/Core.h"
#include "DEngine/Renderer/Buffers/Buffer.h"

#include "stdint.h"

namespace DEngine
{
	class D_API VulkanVertexBuffer : public VertexBuffer
	{
	public:
		VulkanVertexBuffer(float* verts, uint32_t size);
		virtual ~VulkanVertexBuffer();

		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }
		virtual const BufferLayout& GetLayout() const override { return m_Layout; }

		virtual void Bind() const override;
		virtual void Unbind() const override;
	private:
		uint32_t m_RenderID;
		BufferLayout m_Layout;
	};

	class D_API VulkanIndexBuffer : public IndexBuffer
	{
	public:
		VulkanIndexBuffer(uint32_t* inds, uint32_t count);
		virtual ~VulkanIndexBuffer();

		virtual uint32_t GetCount() const { return m_Count; }

		virtual void Bind() const override;
		virtual void Unbind() const override;
	private:
		uint32_t m_RenderID;
		uint32_t m_Count;
	};
}
