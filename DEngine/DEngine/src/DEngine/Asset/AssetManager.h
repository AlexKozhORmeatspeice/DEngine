#pragma once
#include "DEngine/Core.h"

#include "AssetManagerBase.h"

#include "memory"

namespace DEngine
{
	class AssetManager
	{
	public:
		static void SetAssetManager(const std::shared_ptr<BaseAssetManager>& manager);
		static const AssetHandle& CreateAsset(const AssetMetadata& metadata);

		template<typename T>
		static Ref<T> GetAsset(AssetHandle handle)
		{
			return std::static_pointer_cast<T>(m_AssetManager->GetAsset(handle));
		}
	private:
		static std::shared_ptr<BaseAssetManager> m_AssetManager;
	};
}
