#pragma once
#include "DEngine/Core.h"

#include "AssetMetadata.h"
#include "AssetManagerBase.h"
#include "FileWatcher.h"
#include "DEngine/Asset/Asset.h"
#include "DEngine/Jobs/JobSystem.h"
#include "DEngine/Renderer/Texture/Texture.h"

#include "unordered_map"
#include <condition_variable>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <vector>
#include <variant>

namespace DEngine
{
	using AssetRegistry = std::unordered_map<AssetHandle, AssetMetadata>;

	class EditorAssetManager : public BaseAssetManager
	{
	public:
		EditorAssetManager();
		~EditorAssetManager();

		virtual void Init() override;
		virtual void Shutdown() override;

		virtual const AssetHandle& CreateAsset(const std::filesystem::path& path) override;
		const AssetHandle& CreateAsset(AssetMetadata metadata) override;

		virtual const AssetHandle& CreateMeshAsset(const BufferLayout& layout, 
												   float* verts, uint32_t vertsSize, 
												   uint32_t* inds, uint32_t indsSize, 
												   const std::filesystem::path& path) override;
		
		virtual const AssetHandle& CreateMaterialAsset(const Ref<Material>& material, const std::filesystem::path& path) override;
		virtual const AssetHandle& CreateModelAsset(const Ref<Model>& model, const std::filesystem::path& path) override;
		virtual const AssetHandle& CreateSceneAsset(const Ref<Scene>& scene, const std::filesystem::path& path) override;

		virtual Ref<Asset> GetAsset(const AssetHandle& handle) override;

		virtual bool IsAssetHandleValid(const AssetHandle& handle) const override;
		virtual bool IsAssetLoaded(const AssetHandle& handle) const override;

		const AssetMetadata& GetMetadata(const AssetHandle& handle);
		AssetHandle GetHandleByPath(const std::filesystem::path& path);

		virtual inline const AssetHandle& GetBaseRendererShader() const override { return m_BaseShaderHandle; }
		virtual const AssetHandle& GetBaseRendererMaterialWireframeHandle() const override { return m_BaseMaterialWireframe; }
		virtual inline const AssetHandle& GetEmptyTextureHandle() const override { return m_EmptyTextureHandle; }
		virtual const AssetHandle& GetPrimitiveMeshHandle(PrimitiveType type) override;

        void ReloadAsset(const AssetHandle& handle);
        void ReloadAssetByPath(const std::filesystem::path& path);
        void EnableHotReload(bool enable) { m_HotReloadEnabled = enable; }
        bool IsHotReloadEnabled() const { return m_HotReloadEnabled; }

		const AssetRegistry& GetAssetRegistry() const { return m_AssetRegistry; }

        void Update();
		void UploadPendingAssets(bool flushAll = false);

	private:
		struct AssetLoadRequest
		{
			std::mutex Mutex;
			std::condition_variable Ready;
			bool Done = false;
			bool ReturnsPlaceholder = false;
			AssetMetadata metadata;
			Ref<Asset> Result;
			Scope<PinnedJob> Decode;
		};

		template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
		template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

		struct PendingTextureUpload
		{
			AssetHandle Handle;
			TextureSpecification Spec;
			DataBuffer Pixels;
			bool Failed = false;
		};

		struct PendingMeshUpload
		{
			AssetHandle Handle;
			BufferLayout Layout;
			MeshData Data;
			bool Failed = false;
		};

		using PendingUpload = std::variant<PendingTextureUpload, PendingMeshUpload>;

		Ref<Asset> WaitForInFlight(const std::shared_ptr<AssetLoadRequest>& load);
		Ref<Asset> GetPlaceholder(const AssetMetadata& metadata);
		void QueueDecodedAsset(AssetHandle handle, const AssetMetadata& path);

		// Handle stored in the registry. Null when this path is not registered.
		const AssetHandle* FindHandleByPath(const std::filesystem::path& path);
		// Inserts or updates the registry and returns the handle stored there.
		const AssetHandle& EmplaceAsset(AssetHandle handle, const AssetMetadata& metadata, const Ref<Asset>& loaded);
		void AddAssetsInDirectory(const std::filesystem::path& path, bool isVisibleInEditor);
		void SerializeAssetRegistry();
		bool DeserializeAssetRegistry();
		std::filesystem::path GetAssetRegistryFilePath();

		void CreateBaseRendererShader();
		void CreateWireframeShader();
		void CreateBaseRendererMaterialWireframe();
		void CreateBaseRendererMaterial();

		void CreateEmptyTexture();
		const AssetHandle& CreateMeshPrimitive(PrimitiveType type);

        void SetupFileWatcher();
        void OnFileChanged(const std::filesystem::path& path);
        void ReloadShader(const AssetHandle& handle, const AssetMetadata& metadata);
        void ReloadTexture(const AssetHandle& handle, const AssetMetadata& metadata);

	private:
		std::unordered_map<PrimitiveType, AssetHandle> m_MeshPrimitives;
		PrimitiveType m_PlaceholderPrimitive = PrimitiveType::Cube;

		AssetHandle m_BaseShaderHandle;
		AssetHandle m_BaseMaterial;

		AssetHandle m_WireframeShaderHandle;
		AssetHandle m_BaseMaterialWireframe;

		AssetHandle m_EmptyTextureHandle;

		AssetRegistry m_AssetRegistry;
		AssetMap m_LoadedAssets;
		mutable std::shared_mutex m_LoadedAssetsMutex;

        FileWatcher m_FileWatcher;
        bool m_HotReloadEnabled = true;
        std::unordered_map<AssetHandle, std::shared_ptr<AssetLoadRequest>> m_InFlightLoads;
		std::mutex m_PendingMutex;
		std::vector<PendingUpload> m_PendingAssets;
		uint32_t m_NextDecodeThread = 0;
        std::unordered_map<std::filesystem::path, AssetHandle> m_PathToHandle;
        std::unordered_map<AssetHandle, std::filesystem::path> m_HandleToPath;
        
        std::unordered_map<AssetHandle, std::filesystem::file_time_type> m_AssetLastLoadTime;
        std::vector<AssetHandle> m_PendingReloads;
	};
}
