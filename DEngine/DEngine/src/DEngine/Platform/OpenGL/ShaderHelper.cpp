#include "dpch.h"
#include "ShaderHelper.h"

#include "cctype"
#include "cstring"
#include "exception"
#include "fstream"

#include "spirv_glsl.hpp"

namespace DEngine
{
	static const char* kShaderCacheDir = "C:/Users/Lenovo/Desktop/Programing/GameDev/DEngine/DEngine/DEditor/resources/cache";

	shaderc_shader_kind ShaderHelper::ShaderStageKind(ShaderType type)
	{
		if (type == ShaderType::Vertex)
			return shaderc_vertex_shader;

		if (type == ShaderType::Fragment)
			return shaderc_fragment_shader;

		D_CORE_ASSERT(false, "Unkown shader type!");
		return shaderc_glsl_infer_from_source;
	}

	std::string ShaderHelper::Trim(const std::string& text)
	{
		size_t start = 0;
		while (start < text.size() && std::isspace(static_cast<unsigned char>(text[start])))
			++start;

		size_t end = text.size();
		while (end > start && std::isspace(static_cast<unsigned char>(text[end - 1])))
			--end;

		return text.substr(start, end - start);
	}

	std::string ShaderHelper::FlattenUniformStructs(std::string source)
	{
		const std::string structToken = "struct ";
		size_t search = 0;
		while (search < source.size())
		{
			size_t structPos = source.find(structToken, search);
			if (structPos == std::string::npos)
				break;

			size_t nameStart = structPos + structToken.size();
			size_t nameEnd = source.find_first_of(" \t\r\n{", nameStart);
			if (nameEnd == std::string::npos)
				break;

			std::string typeName = source.substr(nameStart, nameEnd - nameStart);
			size_t brace = source.find('{', nameEnd);
			size_t braceEnd = brace == std::string::npos ? std::string::npos : source.find('}', brace);
			if (brace == std::string::npos || braceEnd == std::string::npos)
				break;

			size_t structEnd = source.find(';', braceEnd);
			if (structEnd == std::string::npos)
				break;

			std::string uniformToken = "uniform " + typeName + " ";
			size_t uniformPos = source.find(uniformToken, structEnd);
			if (uniformPos == std::string::npos || uniformPos - structEnd > 8)
			{
				search = structEnd + 1;
				continue;
			}

			size_t instStart = uniformPos + uniformToken.size();
			size_t instEnd = source.find(';', instStart);
			if (instEnd == std::string::npos)
				break;

			std::string instance = Trim(source.substr(instStart, instEnd - instStart));
			std::string body = source.substr(brace + 1, braceEnd - brace - 1);
			std::string uniforms;
			size_t member = 0;
			while (member < body.size())
			{
				size_t semi = body.find(';', member);
				if (semi == std::string::npos)
					break;

				std::string declaration = Trim(body.substr(member, semi - member));
				if (!declaration.empty())
					uniforms += "uniform " + declaration + ";\n";

				member = semi + 1;
			}

			source.replace(structPos, instEnd + 1 - structPos, uniforms);

			std::string prefix = instance + ".";
			size_t at = 0;
			while ((at = source.find(prefix, at)) != std::string::npos)
				source.erase(at, prefix.size());

			search = structPos + uniforms.size();
		}

		return source;
	}

	uint64_t ShaderHelper::HashSource(const std::string& source)
	{
		uint64_t hash = 14695981039346656037ull;
		for (unsigned char c : source)
		{
			hash ^= c;
			hash *= 1099511628211ull;
		}
		return hash;
	}

	const char* ShaderHelper::ShaderStageName(ShaderType type)
	{
		if (type == ShaderType::Vertex)
			return "vert";

		if (type == ShaderType::Fragment)
			return "frag";

		return "stage";
	}

	std::string ShaderHelper::CacheFileName(const std::string& name, const char* stage, const char* postfix, const char* extension)
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

	bool ShaderHelper::ReadSpirvCache(const std::filesystem::path& path, uint64_t hash, std::vector<uint32_t>& spirv)
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

	void ShaderHelper::WriteSpirvCache(const std::filesystem::path& path, uint64_t hash, const std::vector<uint32_t>& spirv)
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

	bool ShaderHelper::ReadTextCache(const std::filesystem::path& path, uint64_t hash, std::string& text)
	{
		std::ifstream in(path, std::ios::binary);
		if (!in)
			return false;

		char magic[4] = {};
		uint64_t storedHash = 0;
		uint32_t size = 0;
		in.read(magic, 4);
		in.read(reinterpret_cast<char*>(&storedHash), sizeof(storedHash));
		in.read(reinterpret_cast<char*>(&size), sizeof(size));
		if (!in || std::memcmp(magic, "DOGL", 4) != 0 || storedHash != hash || size == 0)
			return false;

		text.resize(size);
		in.read(text.data(), static_cast<std::streamsize>(size));
		return static_cast<bool>(in);
	}

	void ShaderHelper::WriteTextCache(const std::filesystem::path& path, uint64_t hash, const std::string& text)
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

		const char magic[4] = { 'D', 'O', 'G', 'L' };
		uint32_t size = static_cast<uint32_t>(text.size());
		out.write(magic, 4);
		out.write(reinterpret_cast<const char*>(&hash), sizeof(hash));
		out.write(reinterpret_cast<const char*>(&size), sizeof(size));
		out.write(text.data(), static_cast<std::streamsize>(text.size()));
	}

	std::vector<uint32_t> ShaderHelper::CompileVulkanGlsl(ShaderType type, const std::string& source, const std::string& name)
	{
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

		return std::vector<uint32_t>(result.cbegin(), result.cend());
	}

	std::string ShaderHelper::SpirvToOpenGLGlsl(const std::vector<uint32_t>& spirv)
	{
		try
		{
			spirv_cross::CompilerGLSL compiler(spirv);
			spirv_cross::CompilerGLSL::Options options;
			options.version = 330;
			options.es = false;
			options.vulkan_semantics = false;
			options.enable_420pack_extension = false;
			options.emit_uniform_buffer_as_plain_uniforms = true;
			compiler.set_common_options(options);
			return FlattenUniformStructs(compiler.compile());
		}
		catch (const std::exception& exception)
		{
			D_CORE_ERROR("{0}", exception.what());
			D_CORE_ASSERT(false, "SPIR-V to OpenGL GLSL failed");
			return {};
		}
	}

	std::string ShaderHelper::OpenGLSourceFromVulkan(ShaderType type, const std::string& source, const std::string& name)
	{
		const char* stage = ShaderStageName(type);
		uint64_t hash = HashSource(source);
		std::filesystem::path cacheDir(kShaderCacheDir);
		std::filesystem::path vulkanPath = cacheDir / CacheFileName(name, stage, "_Vulkan11", "spv");
		std::filesystem::path openGLPath = cacheDir / CacheFileName(name, stage, "_OpenGL", "glsl");

		std::string glsl;
		if (ReadTextCache(openGLPath, hash, glsl))
		{
			D_CORE_INFO("Shader '{0}' {1} loaded from OpenGL cache", name, stage);
			return glsl;
		}

		std::vector<uint32_t> spirv;
		if (ReadSpirvCache(vulkanPath, hash, spirv))
		{
			D_CORE_INFO("Shader '{0}' {1} loaded from Vulkan cache", name, stage);
		}
		else
		{
			spirv = CompileVulkanGlsl(type, source, name);
			D_CORE_ASSERT(!spirv.empty(), "Vulkan GLSL produced no SPIR-V");
			WriteSpirvCache(vulkanPath, hash, spirv);
			D_CORE_INFO("Shader '{0}' {1} compiled to Vulkan SPIR-V", name, stage);
		}

		glsl = SpirvToOpenGLGlsl(spirv);
		D_CORE_ASSERT(!glsl.empty(), "SPIR-V produced no OpenGL GLSL");
		WriteTextCache(openGLPath, hash, glsl);
		D_CORE_INFO("Shader '{0}' {1} cached as OpenGL GLSL", name, stage);
		return glsl;
	}

	GLenum ShaderHelper::ShaderTypeToGLType(ShaderType type)
	{
		if (type == ShaderType::Vertex)
			return GL_VERTEX_SHADER;

		if (type == ShaderType::Fragment)
			return GL_FRAGMENT_SHADER;

		D_CORE_ASSERT(false, "Unkown shader type!");
		return GL_NONE;
	}
}
