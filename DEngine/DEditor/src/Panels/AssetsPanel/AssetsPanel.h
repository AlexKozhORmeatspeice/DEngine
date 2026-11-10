#pragma once

#include "imgui/imgui.h"
#include "DEngine.h"
#include "DEngine/Core.h"

#include "filesystem"

namespace DEngine
{
	class AssetsPanel
	{
	public:
		AssetsPanel();

		void SetContext(const Ref<Scene>& scene) { m_Scene = scene; }
		void OnRender();

	private:
		void DrawDirectoryEntry(const std::filesystem::path& path, bool isDirectory);
		Ref<Texture2D> IconFor(AssetType type) const;
		AssetHandle HandleFor(const std::filesystem::path& path, AssetType& outType) const;

	private:
		Ref<Scene> m_Scene;
		std::filesystem::path m_BaseDir;
		std::filesystem::path m_CurrentDir;

		Ref<Texture2D> m_FolderIcon;
		Ref<Texture2D> m_FileIcon;
		Ref<Texture2D> m_TextureIcon;
		Ref<Texture2D> m_MaterialIcon;
		Ref<Texture2D> m_ModelIcon;
		Ref<Texture2D> m_SceneIcon;
	};
}
