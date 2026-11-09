#pragma once

#include "DEngine.h"
#include "Panels/Properties/PropertiesPanel.h"

namespace DEngine
{
	struct AssetDragPayload
	{
		uint64_t Handle = 0;
		uint16_t Type = 0;
	};

	inline const char* AssetPayloadType()
	{
		return "DENGINE_ASSET";
	}

	inline bool SpawnModelInScene(const Ref<Scene>& scene, AssetHandle handle)
	{
		if (!scene || static_cast<uint64_t>(handle) == 0)
			return false;

		Ref<Model> model = AssetManager::GetAsset<Model>(handle);
		if (!model || model->GetRenderData().empty())
		{
			D_CORE_ERROR("Couldn't add model {0} to the scene", AssetHelper::DisplayName(handle));
			return false;
		}

		const std::string name = AssetHelper::DisplayName(handle);
		const auto& parts = model->GetRenderData();
		Entity first;

		for (size_t i = 0; i < parts.size(); ++i)
		{
			const std::string entityName = parts.size() == 1 ? name : name + " " + std::to_string(i);
			Entity entity = scene->CreateEntity(entityName);
			entity.AddComponent<MeshRendererComponent>(parts[i].MeshHandle, parts[i].MaterialHandle);
			if (!first)
				first = entity;
		}

		if (first)
			PropetiesPanel::SetSelectedContext(first);

		return true;
	}

	inline bool AcceptModelDrop(const Ref<Scene>& scene)
	{
		if (!ImGui::BeginDragDropTarget())
			return false;

		bool spawned = false;
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(AssetPayloadType()))
		{
			const auto* data = static_cast<const AssetDragPayload*>(payload->Data);
			if (data && static_cast<AssetType>(data->Type) == AssetType::Model)
				spawned = SpawnModelInScene(scene, data->Handle);
		}

		ImGui::EndDragDropTarget();
		return spawned;
	}

	inline bool AcceptAssetInto(AssetHandle& slot, AssetType accepted)
	{
		if (!ImGui::BeginDragDropTarget())
			return false;

		bool changed = false;
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(AssetPayloadType()))
		{
			const auto* data = static_cast<const AssetDragPayload*>(payload->Data);
			if (data && static_cast<AssetType>(data->Type) == accepted)
			{
				slot = data->Handle;
				changed = true;
			}
		}

		ImGui::EndDragDropTarget();
		return changed;
	}
}
