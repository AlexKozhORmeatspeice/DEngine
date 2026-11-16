#include "dpch.h"
#include "VulkanShaderHelper.h"

namespace DEngine
{
	uint32_t VulkanShaderHelper::ShaderTypeToVkType(ShaderType type)
	{
		return 0;
	}

	std::vector<uint32_t> VulkanShaderHelper::CompileToSpirv(ShaderType type, const std::string& source, const std::string& name)
	{
		return {};
	}

	std::string VulkanShaderHelper::Trim(const std::string& text)
	{
		return {};
	}

	uint64_t VulkanShaderHelper::HashSource(const std::string& source)
	{
		return 0;
	}

	const char* VulkanShaderHelper::ShaderStageName(ShaderType type)
	{
		return "";
	}

	std::string VulkanShaderHelper::CacheFileName(const std::string& name, const char* stage, const char* postfix, const char* extension)
	{
		return {};
	}

	bool VulkanShaderHelper::ReadSpirvCache(const std::filesystem::path& path, uint64_t hash, std::vector<uint32_t>& spirv)
	{
		return false;
	}

	void VulkanShaderHelper::WriteSpirvCache(const std::filesystem::path& path, uint64_t hash, const std::vector<uint32_t>& spirv)
	{
	}
}
