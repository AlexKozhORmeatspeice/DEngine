#pragma once
#include "DEngine/Core.h"

#include "AssetMetadata.h"
#include "AssetManagerBase.h"


namespace DEngine
{
	using AssetRegistry = std::unordered_map<AssetHandle, AssetMetadata>;

	class EditorAssetManager : public BaseAssetManager
	{
	public:
		const AssetHandle& CreateAsset(AssetMetadata metadata) override;
		virtual Ref<Asset> GetAsset(const AssetHandle& handle) override;

		virtual bool IsAssetHandleValid(const AssetHandle& handle) const override;
		virtual bool IsAssetLoaded(const AssetHandle& handle) const override;

		const AssetMetadata& GetMetadata(const AssetHandle& handle);

		void SerializeAssetRegistry();
		bool DeserializeAssetRegistry();
	private:
		AssetRegistry m_AssetRegistry;
		AssetMap m_LoadedAssets;
	};
}
