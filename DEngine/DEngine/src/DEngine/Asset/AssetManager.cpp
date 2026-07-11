#include "dpch.h"
#include "AssetManager.h"

#include "EditorAssetManager.h"

namespace DEngine
{
	std::shared_ptr<BaseAssetManager> AssetManager::m_AssetManager;

	void AssetManager::Init(const std::shared_ptr<BaseAssetManager>& manager)
	{
		m_AssetManager = manager;
	}

	void AssetManager::Update()
	{
		//Добавление Hot Reload доступно только в editor
		std::shared_ptr<EditorAssetManager> editorAsset = std::dynamic_pointer_cast<EditorAssetManager>(m_AssetManager);
		if (editorAsset)
		{
			editorAsset->Update();
		}
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