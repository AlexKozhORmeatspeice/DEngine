#pragma once

#include "DEngine/Renderer/Shader/Shader.h"

#include "glad/glad.h"
#include "shaderc/shaderc.hpp"

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace DEngine
{
	class ShaderHelper
	{
	public:
		static GLenum ShaderTypeToGLType(ShaderType type);
		static std::string OpenGLSourceFromVulkan(ShaderType type, const std::string& source, const std::string& name);

	private:
		static std::string Trim(const std::string& text);
		static std::string FlattenUniformStructs(std::string source);
		static uint64_t HashSource(const std::string& source);
		static const char* ShaderStageName(ShaderType type);
		static shaderc_shader_kind ShaderStageKind(ShaderType type);
		static std::string CacheFileName(const std::string& name, const char* stage, const char* postfix, const char* extension);
		static bool ReadSpirvCache(const std::filesystem::path& path, uint64_t hash, std::vector<uint32_t>& spirv);
		static void WriteSpirvCache(const std::filesystem::path& path, uint64_t hash, const std::vector<uint32_t>& spirv);
		static bool ReadTextCache(const std::filesystem::path& path, uint64_t hash, std::string& text);
		static void WriteTextCache(const std::filesystem::path& path, uint64_t hash, const std::string& text);
		static std::vector<uint32_t> CompileVulkanGlsl(ShaderType type, const std::string& source, const std::string& name);
		static std::string SpirvToOpenGLGlsl(const std::vector<uint32_t>& spirv);
	};
}
