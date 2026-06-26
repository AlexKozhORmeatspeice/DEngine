#pragma once
#include "DEngine/Core.h"
#include "DEngine/Renderer/Shader/Shader.h"

namespace DEngine
{
	class D_API OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& vertexSrc, const std::string& fragSrc);
		virtual ~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;
	private:
		uint32_t m_RendererID;
	};
}
