#pragma once
#include "DEngine/Core.h"

#include "AssetMetadata.h"
#include "AssetManagerBase.h"
#include "FileWatcher.h"
#include "DEngine/Asset/Asset.h"

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

        void ReloadAsset(const AssetHandle& handle);
        void ReloadAssetByPath(const std::filesystem::path& path);
        void EnableHotReload(bool enable) { m_HotReloadEnabled = enable; }
        bool IsHotReloadEnabled() const { return m_HotReloadEnabled; }

        void Update();

	private:
		void CreateBaseRendererShader();
		const AssetHandle& CreateMeshPrimitive(PrimitiveType type);

        void SetupFileWatcher();
        void OnFileChanged(const std::filesystem::path& path);
        void ReloadShader(const AssetHandle& handle, const AssetMetadata& metadata);
        void ReloadMesh(const AssetHandle& handle, const AssetMetadata& metadata);
        void ReloadTexture(const AssetHandle& handle, const AssetMetadata& metadata);

	private:
		std::unordered_map<PrimitiveType, AssetHandle> m_MeshPrimitives;
		AssetHandle m_BaseShaderHandle;

		AssetRegistry m_AssetRegistry;
		AssetMap m_LoadedAssets;

        FileWatcher m_FileWatcher;
        bool m_HotReloadEnabled = true;
        std::unordered_map<std::filesystem::path, AssetHandle> m_PathToHandle;
        std::unordered_map<AssetHandle, std::filesystem::path> m_HandleToPath;
        
        std::unordered_map<AssetHandle, std::filesystem::file_time_type> m_AssetLastLoadTime;
	};
}
