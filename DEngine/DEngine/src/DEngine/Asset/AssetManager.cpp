#include "dpch.h"
#include "AssetManager.h"

namespace DEngine
{
	std::shared_ptr<BaseAssetManager> AssetManager::m_AssetManager;

	void AssetManager::SetAssetManager(const std::shared_ptr<BaseAssetManager>& manager)
	{
		m_AssetManager = manager;
	}

	const AssetHandle& AssetManager::CreateAsset(const AssetMetadata& metadata)
	{
		auto& handle = m_AssetManager->CreateAsset(metadata);
		return handle;
	}
}