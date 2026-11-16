#pragma once

#include "DEngine/Renderer/Shader/Shader.h"

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace DEngine
{
	class VulkanShaderHelper
	{
	public:
		static uint32_t ShaderTypeToVkType(ShaderType type);
		static std::vector<uint32_t> CompileToSpirv(ShaderType type, const std::string& source, const std::string& name);

	private:
		static std::string Trim(const std::string& text);
		static uint64_t HashSource(const std::string& source);
		static const char* ShaderStageName(ShaderType type);
		static std::string CacheFileName(const std::string& name, const char* stage, const char* postfix, const char* extension);
		static bool ReadSpirvCache(const std::filesystem::path& path, uint64_t hash, std::vector<uint32_t>& spirv);
		static void WriteSpirvCache(const std::filesystem::path& path, uint64_t hash, const std::vector<uint32_t>& spirv);
	};
}
