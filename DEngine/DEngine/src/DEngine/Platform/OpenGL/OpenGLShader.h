#pragma once
#include "DEngine/Core.h"
#include "DEngine/Renderer/Shader/Shader.h"

#include "unordered_map"
#include "glad/glad.h"
#include "glm/glm.hpp"

namespace DEngine
{
	class D_API OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const ShaderRegistry& shaderRegistry, const std::string& name = "");
		virtual ~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void UploadUniformInt(const std::string& name, int val) override;
		virtual void UploadUniformInt(const uint32_t& name, int val) override;
		virtual void UploadUniformFloat(const std::string& name, float val) override;
		virtual void UploadUniformFloat(const uint32_t& name, float val) override;
		virtual void UploadUniformFloat2(const std::string& name, const glm::vec2& val) override;
		virtual void UploadUniformFloat2(const uint32_t& name, const glm::vec2& val) override;
		virtual void UploadUniformFloat3(const std::string& name, const glm::vec3& val) override;
		virtual void UploadUniformFloat3(const uint32_t& name, const glm::vec3& val) override;
		virtual void UploadUniformFloat4(const std::string& name, const glm::vec4& val) override;
		virtual void UploadUniformFloat4(const uint32_t& name, const glm::vec4& val) override;
		virtual void UploadUniformMat3(const std::string& name, const glm::mat3& mat) override;
		virtual void UploadUniformMat3(const uint32_t& name, const glm::mat3& mat) override;
		virtual void UploadUniformMat4(const std::string& name, const glm::mat4& mat) override;
		virtual void UploadUniformMat4(const uint32_t& name, const glm::mat4& mat) override;

		virtual uint32_t GetUniformLocation(const std::string& name) override;

		virtual inline const std::string& GetName() const override { return m_Name; }

	private:
		void Compile(const ShaderRegistry& shaderSources);
	private:
		uint32_t m_RendererID;
		std::string m_Name;
	};
}
