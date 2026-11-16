#include "dpch.h"
#include "VulkanShader.h"

namespace DEngine
{
	VulkanShader::VulkanShader(const ShaderRegistry& shaderRegistry, const std::string& name)
		: m_Name(name)
	{
	}

	VulkanShader::~VulkanShader()
	{
	}

	void VulkanShader::Bind() const
	{
	}

	void VulkanShader::Unbind() const
	{
	}

	void VulkanShader::Compile(const ShaderRegistry& shaderSources)
	{
	}

	void VulkanShader::UploadUniformInt(const std::string& name, int val)
	{
	}

	void VulkanShader::UploadUniformInt(const uint32_t& name, int val)
	{
	}

	void VulkanShader::UploadUniformFloat(const std::string& name, float val)
	{
	}

	void VulkanShader::UploadUniformFloat(const uint32_t& name, float val)
	{
	}

	void VulkanShader::UploadUniformFloat2(const std::string& name, const glm::vec2& val)
	{
	}

	void VulkanShader::UploadUniformFloat2(const uint32_t& name, const glm::vec2& val)
	{
	}

	void VulkanShader::UploadUniformFloat3(const std::string& name, const glm::vec3& val)
	{
	}

	void VulkanShader::UploadUniformFloat3(const uint32_t& name, const glm::vec3& val)
	{
	}

	void VulkanShader::UploadUniformFloat4(const std::string& name, const glm::vec4& val)
	{
	}

	void VulkanShader::UploadUniformFloat4(const uint32_t& name, const glm::vec4& val)
	{
	}

	void VulkanShader::UploadUniformMat3(const std::string& name, const glm::mat3& mat)
	{
	}

	void VulkanShader::UploadUniformMat3(const uint32_t& name, const glm::mat3& mat)
	{
	}

	void VulkanShader::UploadUniformMat4(const std::string& name, const glm::mat4& mat)
	{
	}

	void VulkanShader::UploadUniformMat4(const uint32_t& name, const glm::mat4& mat)
	{
	}

	uint32_t VulkanShader::GetUniformLocation(const std::string& name)
	{
		return 0;
	}
}
