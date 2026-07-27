#include "AssetsPanel.h"

#include "DEngine/Asset/Importer/TextureImporter.h"
#include "DEngine/Asset/AssetManager.h"
#include "DEngine/Asset/EditorAssetManager.h"

namespace DEngine
{
	AssetsPanel::AssetsPanel()
		: m_BaseDir(Project::GetAssetRegistryPath()), m_CurrentDir(m_BaseDir), m_RootNode("/")
	{
		m_DirIcon = TextureImporter::LoadTexture2D("resources/icons/dir_icon.png");
		m_FileIcon = TextureImporter::LoadTexture2D("resources/icons/file_icon.png");
	}

	void AssetsPanel::OnImGuiRender()
	{
		ImGui::Begin("Assets");
		
		RefreshAssetTree();

		ImGui::End();
	}

	void AssetsPanel::RefreshAssetTree()
	{
		auto editorAssets = AssetManager::Get<EditorAssetManager>();
		if (editorAssets == nullptr) return;

		const auto& assetRegistry = editorAssets->GetAssetRegistry();
		for (const auto& [handle, metadata] : assetRegistry)
		{
			for (const auto& p : metadata.FilePath)
			{
				
			}
		}
	}
}