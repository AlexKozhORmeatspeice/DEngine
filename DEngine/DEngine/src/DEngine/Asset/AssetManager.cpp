#include "dpch.h"
#include "AssetManager.h"

namespace DEngine
{
	std::shared_ptr<BaseAssetManager> AssetManager::m_AssetManager;

	void AssetManager::Init(const std::shared_ptr<BaseAssetManager>& manager)
	{
		m_AssetManager = manager;
	}

	const AssetHandle& AssetManager::CreateAsset(const AssetMetadata& metadata)
	{
		auto& handle = m_AssetManager->CreateAsset(metadata);
		return handle;
	}

	const AssetHandle& AssetManager::GetBaseRendererShaderHandle()
	{
		return m_AssetManager->GetBaseRendererShader();
	}
	
	const AssetHandle& AssetManager::CreateMeshAsset(const BufferLayout& layout,
													 float* verts, uint32_t vertsSize,
													 uint32_t* inds, uint32_t indsSize,
													 const std::filesystem::path& path)
	{
		return m_AssetManager->CreateMeshAsset(layout, verts, vertsSize, inds, indsSize, path);
	}

	const AssetHandle& AssetManager::GetPrimitiveMesh(PrimitiveType type)
	{
		return m_AssetManager->GetPrimitiveMesh(type);
	}
}