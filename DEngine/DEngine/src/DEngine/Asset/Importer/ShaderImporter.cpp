#include "dpch.h"
#include "ShaderImporter.h"

namespace DEngine
{
	Ref<Shader> ShaderImporter::ImportShader(AssetHandle handle, const AssetMetadata& metadata)
	{
		return LoadShader(metadata.FilePath);
	}
	
	Ref<Shader> ShaderImporter::LoadShader(const std::filesystem::path& path)
	{
		std::string shaderSource = ReadFile(path.string());
		const ShaderRegistry& shaderSources = Preprocess(shaderSource);

		//Extract name from file path
		std::string filePath = path.string();

		auto lastSlash = filePath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;

		auto lastDot = filePath.rfind(".");
		auto count = lastDot == std::string::npos ? filePath.size() - lastSlash : lastDot - lastSlash;

		std::string name = filePath.substr(lastSlash, count);

		return Shader::Create(shaderSources, name);
	}

	static ShaderType ShaderTypeFromString(const std::string& str)
	{
		if (str == "vertex")
			return ShaderType::Vertex;

		if (str == "fragment" || str == "pixel")
			return ShaderType::Fragment;

		D_CORE_ASSERT(false, "Unkown shader type!");
		return ShaderType::None;
	}

	std::string ShaderImporter::ReadFile(const std::string& filePath)
	{
		std::string res;
		std::ifstream in(filePath, std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			res.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&res[0], res.size());
			in.close();
		}
		else
		{
			D_CORE_ERROR("Can't load shader by path {0}", filePath);
		}
		
		return res;
	}

	const ShaderRegistry ShaderImporter::Preprocess(const std::string& source)
	{
		ShaderRegistry shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLenght = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			size_t lineEnd = source.find('\n', pos);
			D_CORE_ASSERT(lineEnd != std::string::npos, "Shader syntax error");

			size_t typeEnd = lineEnd;
			if (typeEnd > pos && source[typeEnd - 1] == '\r')
				--typeEnd;

			size_t begin = pos + typeTokenLenght + 1;
			D_CORE_ASSERT(begin <= typeEnd, "Shader syntax error");
			std::string type = source.substr(begin, typeEnd - begin);
			ShaderType shaderType = ShaderTypeFromString(type);
			D_CORE_ASSERT(shaderType != ShaderType::None, "Invalid shader type");

			size_t nextLinePos = lineEnd + 1;
			pos = source.find(typeToken, nextLinePos);
			size_t sourceEnd = pos == std::string::npos ? source.size() : pos;
			shaderSources[shaderType] = source.substr(nextLinePos, sourceEnd - nextLinePos);
		}

		return shaderSources;
	}
}