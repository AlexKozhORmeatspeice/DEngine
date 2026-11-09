#include "ScenePanel.h"

#include "entt/include/entt.hpp"
#include "DEngine.h"
#include "Panels/Properties/PropertiesPanel.h"
#include "Panels/AssetDragDrop.h"

#include "ImGuizmo.h"

namespace DEngine
{
	ScenePanel::ScenePanel(const Ref<Scene>& scene)
	{
		SetContext(scene);
	}

	void ScenePanel::SetContext(const Ref<Scene>& scene)
	{
		m_Context = scene;
		m_SelectionContext = {};
	}

	void ScenePanel::OnImGuiRender()
	{
		ImGui::Begin("Scene");

		for (const Entity& entity : m_Context->GetAllEntities())
		{
			DrawEntityNode(entity);
		}

		if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered())
			SetSelected({});

		const ImGuiHoveredFlags hover = ImGuiHoveredFlags_AllowWhenBlockedByActiveItem;
		if (ImGui::IsWindowHovered(hover) && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		{
			if (const ImGuiPayload* payload = ImGui::GetDragDropPayload())
			{
				if (payload->IsDataType(AssetPayloadType()))
				{
					const auto* data = static_cast<const AssetDragPayload*>(payload->Data);
					if (data && static_cast<AssetType>(data->Type) == AssetType::Model)
						SpawnModelInScene(m_Context, data->Handle);
				}
			}
		}

		if (ImGui::BeginPopupContextWindow(0, 1))
		{
			if (ImGui::MenuItem("Create empty entity"))
			{
				m_Context->CreateEntity();
			}
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete entity"))
			{
				m_Context->DestroyEntity(m_SelectionContext);
			}
			ImGui::EndPopup();
		}

		ImGui::End();
	}
	
	void ScenePanel::DrawEntityNode(const Entity& entity)
	{
		if (!entity) return;

		auto& tc = entity.GetComponent<TagComponent>();

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		if (m_SelectionContext == entity)
			flags |= ImGuiTreeNodeFlags_Selected;

		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tc.Tag.c_str());
		if (ImGui::IsItemClicked())
			SetSelected(entity);

		if (opened)
		{
			ImGui::TreePop();
		}
	}

	void ScenePanel::SetSelected(Entity entity)
	{
		if (m_Context)
		{
			auto debugView = m_Context->View<DebugComponent>();
			for (auto [handle, debug] : debugView.each())
			{
				(void)handle;
				debug.debugOn = false;
			}
		}

		m_SelectionContext = entity;

		if (m_SelectionContext && m_SelectionContext.HasComponent<ColliderComponent>())
		{
			if (!m_SelectionContext.HasComponent<DebugComponent>())
				m_SelectionContext.AddComponent<DebugComponent>();
			m_SelectionContext.GetComponent<DebugComponent>().debugOn = true;
		}

		PropetiesPanel::SetSelectedContext(m_SelectionContext);
	}
}