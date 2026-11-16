#include "dpch.h"
#include "VulkanShaderHelper.h"

#include "cctype"
#include "cstring"
#include "fstream"

#include "shaderc/shaderc.hpp"

namespace DEngine
{
	static const char* kShaderCacheDir = "C:/Users/Lenovo/Desktop/Programing/GameDev/DEngine/DEngine/DEditor/resources/cache";

	static shaderc_shader_kind ShaderStageKind(ShaderType type)
	{
		if (type == ShaderType::Vertex)
			return shaderc_vertex_shader;

		if (type == ShaderType::Fragment)
			return shaderc_fragment_shader;

		D_CORE_ASSERT(false, "Unkown shader type!");
		return shaderc_glsl_infer_from_source;
	}

	uint32_t VulkanShaderHelper::ShaderTypeToVkType(ShaderType type)
	{
		if (type == ShaderType::Vertex)
			return VK_SHADER_STAGE_VERTEX_BIT;

		if (type == ShaderType::Fragment)
			return VK_SHADER_STAGE_FRAGMENT_BIT;

		D_CORE_ASSERT(false, "Unkown shader type!");
		return 0;
	}

	std::string VulkanShaderHelper::Trim(const std::string& text)
	{
		size_t start = 0;
		while (start < text.size() && std::isspace(static_cast<unsigned char>(text[start])))
			++start;

		size_t end = text.size();
		while (end > start && std::isspace(static_cast<unsigned char>(text[end - 1])))
			--end;

		return text.substr(start, end - start);
	}

	uint64_t VulkanShaderHelper::HashSource(const std::string& source)
	{
		uint64_t hash = 14695981039346656037ull;
		for (unsigned char c : source)
		{
			hash ^= c;
			hash *= 1099511628211ull;
		}
		return hash;
	}

	const char* VulkanShaderHelper::ShaderStageName(ShaderType type)
	{
		if (type == ShaderType::Vertex)
			return "vert";

		if (type == ShaderType::Fragment)
			return "frag";

		return "stage";
	}

	std::string VulkanShaderHelper::CacheFileName(const std::string& name, const char* stage, const char* postfix, const char* extension)
	{
		std::string safe = name.empty() ? "Shader" : name;
		for (char& c : safe)
		{
			unsigned char uc = static_cast<unsigned char>(c);
			if (std::isalnum(uc) || c == '_' || c == '-' || c == '.')
				continue;
			c = '_';
		}
		return safe + "." + stage + postfix + "." + extension;
	}

	bool VulkanShaderHelper::ReadSpirvCache(const std::filesystem::path& path, uint64_t hash, std::vector<uint32_t>& spirv)
	{
		std::ifstream in(path, std::ios::binary);
		if (!in)
			return false;

		char magic[4] = {};
		uint64_t storedHash = 0;
		uint32_t wordCount = 0;
		in.read(magic, 4);
		in.read(reinterpret_cast<char*>(&storedHash), sizeof(storedHash));
		in.read(reinterpret_cast<char*>(&wordCount), sizeof(wordCount));
		if (!in || std::memcmp(magic, "DSPV", 4) != 0 || storedHash != hash || wordCount == 0)
			return false;

		spirv.resize(wordCount);
		in.read(reinterpret_cast<char*>(spirv.data()), static_cast<std::streamsize>(wordCount * sizeof(uint32_t)));
		return static_cast<bool>(in);
	}

	void VulkanShaderHelper::WriteSpirvCache(const std::filesystem::path& path, uint64_t hash, const std::vector<uint32_t>& spirv)
	{
		std::error_code error;
		std::filesystem::create_directories(path.parent_path(), error);
		if (error)
		{
			D_CORE_ERROR("Can't create shader cache folder {0}", path.parent_path().string());
			return;
		}

		std::ofstream out(path, std::ios::binary | std::ios::trunc);
		if (!out)
		{
			D_CORE_ERROR("Can't write shader cache {0}", path.string());
			return;
		}

		const char magic[4] = { 'D', 'S', 'P', 'V' };
		uint32_t wordCount = static_cast<uint32_t>(spirv.size());
		out.write(magic, 4);
		out.write(reinterpret_cast<const char*>(&hash), sizeof(hash));
		out.write(reinterpret_cast<const char*>(&wordCount), sizeof(wordCount));
		out.write(reinterpret_cast<const char*>(spirv.data()), static_cast<std::streamsize>(spirv.size() * sizeof(uint32_t)));
	}

	std::vector<uint32_t> VulkanShaderHelper::CompileToSpirv(ShaderType type, const std::string& source, const std::string& name)
	{
		const char* stage = ShaderStageName(type);
		uint64_t hash = HashSource(source);
		std::filesystem::path vulkanPath = std::filesystem::path(kShaderCacheDir) / CacheFileName(name, stage, "_Vulkan11", "spv");

		std::vector<uint32_t> spirv;
		if (ReadSpirvCache(vulkanPath, hash, spirv))
		{
			D_CORE_INFO("Shader '{0}' {1} loaded from Vulkan cache", name, stage);
			return spirv;
		}

		shaderc::Compiler compiler;
		D_CORE_ASSERT(compiler.IsValid(), "shaderc compiler failed to start");

		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_1);
		options.SetSourceLanguage(shaderc_source_language_glsl);
		options.SetOptimizationLevel(shaderc_optimization_level_zero);

		shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(source, ShaderStageKind(type), name.c_str(), options);
		if (result.GetCompilationStatus() != shaderc_compilation_status_success)
		{
			D_CORE_ERROR("{0}", result.GetErrorMessage());
			D_CORE_ASSERT(false, "Vulkan GLSL compilation error");
			return {};
		}

		spirv.assign(result.cbegin(), result.cend());
		D_CORE_ASSERT(!spirv.empty(), "Vulkan GLSL produced no SPIR-V");
		WriteSpirvCache(vulkanPath, hash, spirv);
		D_CORE_INFO("Shader '{0}' {1} compiled to Vulkan SPIR-V", name, stage);
		return spirv;
	}
}
