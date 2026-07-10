#include "dpch.h"

#include "AssetImporter.h"
#include "TextureImporter.h"

#include "unordered_map"

namespace DEngine
{
	using AssetImportFunc = std::function<Ref<Asset>(AssetHandle, const AssetMetadata&)>;
	static std::map<AssetType, AssetImportFunc> s_AssetImportFuncs = {
		{AssetType::Texture2D, TextureImporter::ImportTexture2D}
	};

	Ref<Asset> AsssetImporter::ImportAsset(AssetHandle assetHandle, const AssetMetadata& metadata)
	{
		auto it = s_AssetImportFuncs.find(metadata.Type);
		if (it == s_AssetImportFuncs.end())
		{
			D_CORE_ERROR("Can't process data of type: {0}", (uint16_t)metadata.Type);
			return nullptr;
		}

		return it->second(assetHandle, metadata);
	}
}
