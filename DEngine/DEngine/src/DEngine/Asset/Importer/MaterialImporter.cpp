#include "dpch.h"
#include "MaterialImporter.h"

#include "DEngine/Asset/AssetManager.h"
#include "DEngine/Asset/Serializer/MaterialSerializer.h"

namespace DEngine
{
	Ref<Material> MaterialImporter::ImportMaterial(AssetHandle handle, const AssetMetadata& metadata)
	{
		return LoadMaterial(metadata.FilePath);
	}

	Ref<Material> MaterialImporter::LoadMaterial(const std::filesystem::path& path)
	{
		DeserializeMaterialResult& desRes = MaterialSerializer::Deserialize(path);
		Ref<Material> resMat = nullptr;

		if (desRes.isSuccessful)
		{
			resMat = desRes.material;
		}
		else
		{
			resMat = nullptr;
			D_CORE_ERROR("Couldn't find material by path {0}", path.string());
		}

		return resMat;
	}
}
