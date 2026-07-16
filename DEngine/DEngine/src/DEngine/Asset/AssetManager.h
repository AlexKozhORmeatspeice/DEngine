#pragma once
#include "DEngine/Core.h"

#include "AssetManagerBase.h"

#include "memory"

namespace DEngine
{
	class AssetManager
	{
	public:
		static void Init(const std::shared_ptr<BaseAssetManager>& manager);
		static const AssetHandle& CreateAsset(const std::filesystem::path& path);
		static const AssetHandle& CreateAsset(const AssetMetadata& metadata);

		static void Update();

		template<typename T>
		static Ref<T> GetAsset(AssetHandle handle)
		{
			return std::dynamic_pointer_cast<T>(m_AssetManager->GetAsset(handle));
		}

		static const AssetHandle& CreateMeshAsset(const BufferLayout& layout, 
												  float* verts, uint32_t vertsSize, 
												  uint32_t* inds, uint32_t indsSize, 
												  const std::filesystem::path& path);

		static const AssetHandle& GetBaseRendererShaderHandle();
		static const AssetHandle& GetPrimitiveMesh(PrimitiveType type);

	private:
		static std::shared_ptr<BaseAssetManager> m_AssetManager;
	};
}
