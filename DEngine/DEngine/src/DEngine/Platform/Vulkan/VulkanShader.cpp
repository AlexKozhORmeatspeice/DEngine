#include "dpch.h"
#include "VulkanShader.h"
#include "VulkanShaderHelper.h"
#include "DEngine/Application.h"

namespace DEngine
{
	VulkanShader::VulkanShader(const ShaderRegistry& shaderRegistry, const std::string& name)
		: m_Name(name)
	{
		Compile(shaderRegistry);
	}

	VulkanShader::~VulkanShader()
	{
		auto device = static_cast<VkDevice*>(Application::Get().GetDevice().GetNativeDevice());
		for (auto&& [type, shaderModule] : m_ShaderModules)
		{
			if (shaderModule != VK_NULL_HANDLE)
				vkDestroyShaderModule(*device, shaderModule, nullptr);
		}
	}

	void VulkanShader::Bind() const
	{
	}

	void VulkanShader::Unbind() const
	{
	}

	void VulkanShader::Compile(const ShaderRegistry& shaderSources)
	{
		auto device = static_cast<VkDevice*>(Application::Get().GetDevice().GetNativeDevice());

		for (auto&& [type, source] : shaderSources)
		{
			std::vector<uint32_t> spirv = VulkanShaderHelper::CompileToSpirv(type, source, m_Name);
			D_CORE_ASSERT(!spirv.empty(), "Vulkan GLSL produced no SPIR-V");

			VkShaderModule shaderModule = VK_NULL_HANDLE;
			VkShaderModuleCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.codeSize = spirv.size() * sizeof(uint32_t);
			createInfo.pCode = spirv.data();
			D_CORE_ASSERT(vkCreateShaderModule(*device, &createInfo, nullptr, &shaderModule) == VK_SUCCESS, "Failed to create shader module");
			m_ShaderModules[type] = shaderModule;
		}
	}

	void VulkanShader::UploadUniformInt(const std::string& name, int val)
	{
		//WIP
	}

	void VulkanShader::UploadUniformInt(const uint32_t& name, int val)
	{
		//WIP
	}

	void VulkanShader::UploadUniformFloat(const std::string& name, float val)
	{
		//WIP
	}

	void VulkanShader::UploadUniformFloat(const uint32_t& name, float val)
	{
		//WIP
	}

	void VulkanShader::UploadUniformFloat2(const std::string& name, const glm::vec2& val)
	{
		//WIP
	}

	void VulkanShader::UploadUniformFloat2(const uint32_t& name, const glm::vec2& val)
	{
		//WIP
	}

	void VulkanShader::UploadUniformFloat3(const std::string& name, const glm::vec3& val)
	{
		//WIP
	}

	void VulkanShader::UploadUniformFloat3(const uint32_t& name, const glm::vec3& val)
	{
		//WIP
	}

	void VulkanShader::UploadUniformFloat4(const std::string& name, const glm::vec4& val)
	{
		//WIP
	}

	void VulkanShader::UploadUniformFloat4(const uint32_t& name, const glm::vec4& val)
	{
		//WIP
	}

	void VulkanShader::UploadUniformMat3(const std::string& name, const glm::mat3& mat)
	{
		//WIP
	}

	void VulkanShader::UploadUniformMat3(const uint32_t& name, const glm::mat3& mat)
	{
		//WIP
	}

	void VulkanShader::UploadUniformMat4(const std::string& name, const glm::mat4& mat)
	{
		//WIP
	}

	void VulkanShader::UploadUniformMat4(const uint32_t& name, const glm::mat4& mat)
	{
		//WIP
	}

	uint32_t VulkanShader::GetUniformLocation(const std::string& name)
	{
		//WIP
		return 0;
	}
}
