#include "dpch.h"
#include "Shader.h"

#include "DEngine/Platform/OpenGL/OpenGLShader.h"
#include "DEngine/Renderer/Renderer.h"

namespace DEngine
{
	Shader* Shader::Create(const std::string& vertexSrc, const std::string& fragSrc)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::OpenGL:
			return new OpenGLShader(vertexSrc, fragSrc);
		case RendererAPI::None: D_CORE_ASSERT(false, "None API is not supported");
		default:
			break;
		}
		
		D_CORE_ASSERT(false, "Current API doesn't support shaders");
		return nullptr;
	}
}