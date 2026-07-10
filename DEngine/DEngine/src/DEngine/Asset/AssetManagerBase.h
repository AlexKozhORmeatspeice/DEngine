#pragma once
#include "DEngine/Core.h"
#include "Asset.h"

#include "AssetMetadata.h"
#include "unordered_map"

namespace DEngine
{
	using AssetMap = std::unordered_map<AssetHandle, Ref<Asset>>;

	class BaseAssetManager
	{
	public:
		virtual const AssetHandle& CreateAsset(AssetMetadata metadata) = 0;
		virtual Ref<Asset> GetAsset(const AssetHandle& handle) = 0;

		virtual bool IsAssetHandleValid(const AssetHandle& handle) const = 0;
		virtual bool IsAssetLoaded(const AssetHandle& handle) const = 0;
	};
}
