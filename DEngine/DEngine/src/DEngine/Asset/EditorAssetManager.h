#pragma once
#include "DEngine/Core.h"

#include "AssetMetadata.h"
#include "AssetManagerBase.h"

#include "unordered_map"

namespace DEngine
{
	using AssetRegistry = std::unordered_map<AssetHandle, AssetMetadata>;

	class EditorAssetManager : public BaseAssetManager
	{
	public:
		EditorAssetManager();

		const AssetHandle& CreateAsset(AssetMetadata metadata) override;
		virtual const AssetHandle& CreateMeshAsset(const BufferLayout& layout, 
												   float* verts, uint32_t vertsSize, 
												   uint32_t* inds, uint32_t indsSize, 
												   const std::filesystem::path& path) override;

		virtual Ref<Asset> GetAsset(const AssetHandle& handle) override;

		virtual bool IsAssetHandleValid(const AssetHandle& handle) const override;
		virtual bool IsAssetLoaded(const AssetHandle& handle) const override;

		const AssetMetadata& GetMetadata(const AssetHandle& handle);

		virtual inline const AssetHandle& GetBaseRendererShader() const override { return m_BaseShaderHandle; }
		virtual const AssetHandle& GetPrimitiveMesh(PrimitiveType type) override;

		void SerializeAssetRegistry();
		bool DeserializeAssetRegistry();
	private:

		void CreateBaseRendererShader();
		const AssetHandle& CreateMeshPrimitive(PrimitiveType type);

	private:
		std::unordered_map<PrimitiveType, AssetHandle> m_MeshPrimitives;
		AssetHandle m_BaseShaderHandle;

		AssetRegistry m_AssetRegistry;
		AssetMap m_LoadedAssets;
	};
}
