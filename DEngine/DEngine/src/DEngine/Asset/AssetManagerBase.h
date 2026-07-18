#pragma once
#include "DEngine/Core.h"
#include "Asset.h"

#include "DEngine/Renderer/Mesh/Mesh.h"
#include "DEngine/Renderer/Mesh/MeshGenerator.h"
#include "DEngine/Renderer/Material/Material.h"
#include "DEngine/Renderer/Mesh/Model.h"
#include "DEngine/Scene/Scene.h"

#include "AssetMetadata.h"
#include "unordered_map"
#include "filesystem"

namespace DEngine
{
	using AssetMap = std::unordered_map<AssetHandle, Ref<Asset>>;

	//Нужные какие-то методы для очистки ресурсов при уничтожении всех объектов, ссылающихся на них
	class BaseAssetManager
	{
	public:
		virtual void Init() = 0;
		virtual const AssetHandle& CreateAsset(const std::filesystem::path& path) = 0;
		virtual const AssetHandle& CreateAsset(AssetMetadata metadata) = 0;

		virtual const AssetHandle& CreateMeshAsset(const BufferLayout& layout, 
												   float* verts, uint32_t vertsSize, 
												   uint32_t* inds, uint32_t indsSize, 
												   const std::filesystem::path& path) = 0;
		virtual const AssetHandle& CreateMaterialAsset(const Ref<Material>& material, const std::filesystem::path& path) = 0;
		virtual const AssetHandle& CreateModelAsset(const Ref<Model>& model, const std::filesystem::path& path) = 0;
		virtual const AssetHandle& CreateSceneAsset(const Ref<Scene>& model, const std::filesystem::path& path) = 0;

		virtual Ref<Asset> GetAsset(const AssetHandle& handle) = 0;

		virtual const AssetHandle& GetBaseRendererShader() const = 0;

		virtual const AssetHandle& GetPrimitiveMesh(PrimitiveType type) = 0;

		virtual bool IsAssetHandleValid(const AssetHandle& handle) const = 0;
		virtual bool IsAssetLoaded(const AssetHandle& handle) const = 0;
	};
}
