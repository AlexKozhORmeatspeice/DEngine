#include "dpch.h"
#include "Shader.h"

#include "DEngine/Platform/OpenGL/OpenGLShader.h"
#include "DEngine/Renderer/Renderer.h"

namespace DEngine
{
	Ref<Shader> Shader::Create(const ShaderRegistry& shaderRegistry, const std::string& name)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::OpenGL:
				return std::make_shared<OpenGLShader>(shaderRegistry, name);
			case RendererAPI::API::None: D_CORE_ASSERT(false, "None API is not supported");
			default:
				break;
		}
		
		D_CORE_ASSERT(false, "Current API doesn't support shaders");
		return nullptr;
	}
}