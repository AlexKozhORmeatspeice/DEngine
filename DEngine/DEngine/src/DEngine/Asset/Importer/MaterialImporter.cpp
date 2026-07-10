#include "dpch.h"
#include "MaterialImporter.h"

#include "DEngine/Asset/AssetManager.h"

namespace DEngine
{
	Ref<Material> MaterialImporter::ImportMaterial(AssetHandle handle, const AssetMetadata& metadata)
	{
		return LoadMaterial(metadata.FilePath);
	}

	//TODO: временно решение. В будущем тут просто должна происходить десериализация из какого-нибудь YAML файла материалов
	Ref<Material> MaterialImporter::LoadMaterial(const std::filesystem::path& path)
	{
		return CreateRef<Material>(AssetManager::GetBaseRendererShaderHandle());
	}
}
