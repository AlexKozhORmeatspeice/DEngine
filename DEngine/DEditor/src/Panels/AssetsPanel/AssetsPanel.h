#pragma once

#include "imgui/imgui.h"
#include "DEngine.h"
#include "DEngine/Core.h"

#include "map"
#include "unordered_map"
#include "vector"
#include "memory"

namespace DEngine
{
	struct TreeNode
	{
		std::filesystem::path Path;
		std::set<TreeNode*> Children;
		TreeNode* Parent = nullptr;

		TreeNode(const std::filesystem::path& path)
			: Path(path)
		{
		}
	};

    class AssetsPanel
    {
    public:
        AssetsPanel();

        void OnImGuiRender();
    private:
        void RefreshAssetTree();
        void BuildAssetCache(const std::shared_ptr<EditorAssetManager>& editorAssets);
        void ClearTree(TreeNode& node);
    private:
        std::filesystem::path m_BaseDir;
        std::filesystem::path m_CurrentDir;

        TreeNode m_RootNode;

        std::unordered_map<std::filesystem::path, std::vector<AssetMetadata>> m_CachedAssets;
        std::vector<std::filesystem::path> m_CurrentDirs;
        std::vector<AssetMetadata> m_CurrentAssets;

        Ref<Texture2D> m_DirIcon;
        Ref<Texture2D> m_FileIcon;
        bool m_NeedsRefresh = true;
    };
}