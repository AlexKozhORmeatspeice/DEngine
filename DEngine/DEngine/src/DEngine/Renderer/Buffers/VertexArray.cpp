#include "dpch.h"
#include "VertexArray.h"
#include "DEngine/Application.h"
#include "DEngine/Renderer/Renderer.h"
#include "DEngine/Platform/OpenGL/OpenGLVertexArray.h"

namespace DEngine
{
	VertexArray* VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::OpenGL: return new OpenGLVertexArray();
			case RendererAPI::API::None: D_CORE_ASSERT(false, "None API is not supported");
			default:
				break;
		}

		D_CORE_ASSERT(false, "Current Renderer API doesn't support vertex array");
		return nullptr;
	}
}