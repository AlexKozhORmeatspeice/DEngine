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

		virtual void UploadUniformInt(const std::string& name, int val) override;
		virtual void UploadUniformFloat(const std::string& name, float val) override;
		virtual void UploadUniformFloat2(const std::string& name, const glm::vec2& val) override;
		virtual void UploadUniformFloat3(const std::string& name, const glm::vec3& val) override;
		virtual void UploadUniformFloat4(const std::string& name, const glm::vec4& val) override;
		virtual void UploadUniformMat3(const std::string& name, const glm::mat3& mat) override;
		virtual void UploadUniformMat4(const std::string& name, const glm::mat4& mat) override;
	private:
		uint32_t m_RendererID;
	};
}
