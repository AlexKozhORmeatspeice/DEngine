#pragma once
#include "DEngine/Core.h"

#include "DEngine/Asset/Asset.h"
#include "DEngine/Asset/AssetMetadata.h"

#include "DEngine/Renderer/Material/Material.h"

namespace DEngine
{
	class MaterialImporter
	{
	public:
		static Ref<Material> ImportMaterial(AssetHandle handle, const AssetMetadata& metadata);
		static Ref<Material> LoadMaterial(const std::filesystem::path& path);
	};
}
