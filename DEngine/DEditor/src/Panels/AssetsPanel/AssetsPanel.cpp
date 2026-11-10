#include "AssetsPanel.h"

#include "DEngine/Asset/Importer/TextureImporter.h"
#include "DEngine/Asset/AssetManager.h"
#include "DEngine/Asset/EditorAssetManager.h"
#include "Panels/AssetDragDrop.h"
#include "DEngine/Profiler/Profiler.h"

#include <algorithm>

namespace DEngine
{
	AssetsPanel::AssetsPanel()
		: m_BaseDir(Project::GetAssetRegistryPath()), m_CurrentDir(m_BaseDir)
	{
		m_FolderIcon = TextureImporter::LoadTexture2D("resources/icons/folder_icon.png");
		m_FileIcon = TextureImporter::LoadTexture2D("resources/icons/file_icon.png");
		m_TextureIcon = TextureImporter::LoadTexture2D("resources/icons/texture_icon.png");
		m_MaterialIcon = TextureImporter::LoadTexture2D("resources/icons/material_icon.png");
		m_ModelIcon = TextureImporter::LoadTexture2D("resources/icons/model_icon.png");
		m_SceneIcon = TextureImporter::LoadTexture2D("resources/icons/scene_icon.png");
	}

	Ref<Texture2D> AssetsPanel::IconFor(AssetType type) const
	{
		switch (type)
		{
			case AssetType::Texture2D: return m_TextureIcon ? m_TextureIcon : m_FileIcon;
			case AssetType::Material: return m_MaterialIcon ? m_MaterialIcon : m_FileIcon;
			case AssetType::Model: return m_ModelIcon ? m_ModelIcon : m_FileIcon;
			case AssetType::Scene: return m_SceneIcon ? m_SceneIcon : m_FileIcon;
			default: return m_FileIcon;
		}
	}

	AssetHandle AssetsPanel::HandleFor(const std::filesystem::path& path, AssetType& outType) const
	{
		outType = AssetHelper::TypeFromExtension(path);
		auto editor = AssetManager::Get<EditorAssetManager>();
		if (!editor)
			return AssetHandle(static_cast<uint64_t>(AssetHandle::Invalid()));

		AssetHandle handle = editor->GetHandleByPath(path);
		if (static_cast<uint64_t>(handle) == 0 && outType != AssetType::None)
			handle = editor->CreateAsset(path);

		return handle;
	}

	void AssetsPanel::OnRender()
	{
		D_PROFILE_FUNCTION();
		ImGui::Begin("Assets");

		if (!std::filesystem::exists(m_CurrentDir))
			m_CurrentDir = m_BaseDir;

		if (!AssetHelper::SamePath(m_CurrentDir, m_BaseDir))
		{
			if (ImGui::Button("<-"))
				m_CurrentDir = m_CurrentDir.parent_path();
			ImGui::SameLine();
		}

		ImGui::TextUnformatted(m_CurrentDir.generic_string().c_str());
		ImGui::Separator();

		std::vector<std::filesystem::directory_entry> entries;
		if (std::filesystem::exists(m_CurrentDir))
		{
			for (const auto& entry : std::filesystem::directory_iterator(m_CurrentDir))
			{
				if (AssetHelper::IsResourcesPath(entry.path()))
					continue;

				const std::string filename = entry.path().filename().string();
				if (!filename.empty() && filename[0] == '.')
					continue;

				entries.push_back(entry);
			}
		}

		std::sort(entries.begin(), entries.end(), [](const auto& a, const auto& b)
		{
			if (a.is_directory() != b.is_directory())
				return a.is_directory();
			return a.path().filename().string() < b.path().filename().string();
		});

		const float thumbnail = 64.0f;
		const float padding = 16.0f;
		const float cell = thumbnail + padding;
		const float panelWidth = ImGui::GetContentRegionAvail().x;
		int columns = static_cast<int>(panelWidth / cell);
		if (columns < 1)
			columns = 1;

		ImGui::Columns(columns, nullptr, false);

		for (const auto& entry : entries)
			DrawDirectoryEntry(entry.path(), entry.is_directory());

		ImGui::Columns(1);
		ImGui::End();
	}

	void AssetsPanel::DrawDirectoryEntry(const std::filesystem::path& path, bool isDirectory)
	{
		AssetType type = AssetType::None;
		AssetHandle handle;
		if (!isDirectory)
			handle = HandleFor(path, type);

		Ref<Texture2D> icon = isDirectory ? m_FolderIcon : IconFor(type);
		const std::string name = path.filename().string();

		ImGui::PushID(path.generic_string().c_str());
		ImGui::BeginGroup();

		const ImVec2 size(64.0f, 64.0f);
		if (icon)
		{
			ImGui::ImageButton((ImTextureID)(intptr_t)icon->GetRendererID(), size, ImVec2(0, 1), ImVec2(1, 0));
		}
		else
		{
			ImGui::Button(name.c_str(), size);
		}

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && isDirectory)
			m_CurrentDir = path;

		if (!isDirectory && static_cast<uint64_t>(handle) != 0 && type != AssetType::None && ImGui::BeginDragDropSource())
		{
			AssetDragPayload payload;
			payload.Handle = handle;
			payload.Type = static_cast<uint16_t>(type);
			ImGui::SetDragDropPayload(AssetPayloadType(), &payload, sizeof(payload));
			ImGui::TextUnformatted(name.c_str());
			ImGui::EndDragDropSource();
		}

		if (!isDirectory && type == AssetType::Model && ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Add to Scene"))
				SpawnModelInScene(m_Scene, handle);
			ImGui::EndPopup();
		}

		ImGui::TextWrapped("%s", name.c_str());
		ImGui::EndGroup();
		ImGui::PopID();
		ImGui::NextColumn();
	}
}
