#pragma once

#include "DEngine/Core.h"
#include "DEngine/Asset/Asset.h"
#include "DEngine/Asset/AssetMetadata.h"

namespace DEngine
{
	class AsssetImporter
	{
	public:
		static Ref<Asset> ImportAsset(AssetHandle assetHandle, const AssetMetadata& metadata);
	};
}
