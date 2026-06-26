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

		virtual void UploadUniformMat4(const std::string& name, const glm::mat4& mat) override;
	private:
		uint32_t m_RendererID;
	};
}
