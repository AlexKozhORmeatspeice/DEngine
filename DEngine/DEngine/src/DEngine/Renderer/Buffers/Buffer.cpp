#include "dpch.h"
#include "Buffer.h"

#include "DEngine/Renderer/Renderer.h"
#include "DEngine/Platform/OpenGL/OpenGLBuffer.h"

namespace DEngine
{
	VertexBuffer* VertexBuffer::Create(float* verts, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::OpenGL:
			return new OpenGLVertexBuffer(verts, size);
		case RendererAPI::None:
			D_CORE_ASSERT(false, "None API is not supported")
			break;
		}

		D_CORE_ASSERT(false, "Current Renderer API doesn't support vertex buffers");
		return nullptr;
	}


	IndexBuffer* IndexBuffer::Create(uint32_t* inds, uint32_t count)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::OpenGL:
			return new OpenGLIndexBuffer(inds, count);
		case RendererAPI::None:
			D_CORE_ASSERT(false, "None API is not supported")
			break;
		}

		D_CORE_ASSERT(false, "Current Renderer API doesn't support index buffers");
		return nullptr;

	}
}