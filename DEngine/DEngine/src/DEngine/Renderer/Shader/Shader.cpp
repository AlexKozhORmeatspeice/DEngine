#include "dpch.h"
#include "Shader.h"

#include "DEngine/Platform/OpenGL/OpenGLShader.h"
#include "DEngine/Renderer/Renderer.h"

namespace DEngine
{
	Ref<Shader> Shader::Create(const std::string& filePath)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::OpenGL:
				return std::make_shared<OpenGLShader>(filePath);
			case RendererAPI::API::None: D_CORE_ASSERT(false, "None API is not supported");
			default:
				break;
		}
		
		D_CORE_ASSERT(false, "Current API doesn't support shaders");
		return nullptr;

	}
	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragSrc)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::OpenGL:
				return std::make_shared<OpenGLShader>(name, vertexSrc, fragSrc);
			case RendererAPI::API::None: D_CORE_ASSERT(false, "None API is not supported");
			default:
				break;
		}
		
		D_CORE_ASSERT(false, "Current API doesn't support shaders");
		return nullptr;
	}

	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		auto& name = shader->GetName();
		Add(name, shader);
	}

	void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
	{
		D_CORE_ASSERT(m_Shaders.find(name) == m_Shaders.end(), "Shader already exists");
		m_Shaders[name] = shader;
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& filePath)
	{
		Ref<Shader> shader = Shader::Create(filePath);
		Add(shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filePath)
	{
		Ref<Shader> shader = Shader::Create(filePath);
		Add(name, shader);
		return shader;
	}


	Ref<Shader> ShaderLibrary::Get(const std::string& name)
	{
		D_CORE_ASSERT(m_Shaders.find(name) != m_Shaders.end(), "Shader doesn't exist");
		return m_Shaders[name];
	}
}