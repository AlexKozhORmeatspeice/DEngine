#pragma once
#include "DEngine/Core.h"

#include "DEngine/Asset/Asset.h"
#include "DEngine/Asset/AssetMetadata.h"

#include "DEngine/Renderer/Shader/Shader.h"

namespace DEngine
{
	class ShaderImporter
	{
	public:
		static Ref<Shader> ImportShader(AssetHandle handle, const AssetMetadata& metadata);
		static Ref<Shader> LoadShader(const std::filesystem::path& path);
	private:
		static std::string ReadFile(const std::string& filePath);
		static const ShaderRegistry Preprocess(const std::string& source);
	};
}
