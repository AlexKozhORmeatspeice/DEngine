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
			size_t eol = source.find("\r\n", pos);
			D_CORE_ASSERT(eol != std::string::npos, "Shader syntax error");
			
			size_t begin = pos + typeTokenLenght + 1;
			std::string type = source.substr(begin, eol - begin);
			D_CORE_ASSERT(ShaderTypeFromString(type) != ShaderType::None, "Invalid shader type");

			size_t nextLinePos = source.find("\r\n", eol);
			pos = source.find(typeToken, nextLinePos);
			shaderSources[ShaderTypeFromString(type)] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
		}

		return shaderSources;
	}
}