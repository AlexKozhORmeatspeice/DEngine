#include "dpch.h"

#include "AssetImporter.h"
#include "TextureImporter.h"
#include "ShaderImporter.h"
#include "MeshImporter.h"
#include "MaterialImporter.h"
#include "ModelImporter.h"
#include "SceneImporter.h"

#include "unordered_map"

namespace DEngine
{
	using AssetImportFunc = std::function<Ref<Asset>(AssetHandle, const AssetMetadata&)>;
	static std::map<AssetType, AssetImportFunc> s_AssetImportFuncs = {
		{AssetType::Texture2D, TextureImporter::ImportTexture2D},
		{AssetType::Shader, ShaderImporter::ImportShader},
		{AssetType::Mesh, MeshImporter::ImportMesh},
		{AssetType::Material, MaterialImporter::ImportMaterial},
		{AssetType::Model, ModelImporter::ImportModel},
		{AssetType::Scene, SceneImporter::ImportScene},
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
