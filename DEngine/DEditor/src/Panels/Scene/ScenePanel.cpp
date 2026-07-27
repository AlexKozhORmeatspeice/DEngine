#include "ScenePanel.h"

#include "entt/include/entt.hpp"
#include "DEngine.h"
#include "Panels/Properties/PropertiesPanel.h"

namespace DEngine
{
	ScenePanel::ScenePanel(const Ref<Scene>& scene)
	{
		SetContext(scene);
	}

	void ScenePanel::SetContext(const Ref<Scene>& scene)
	{
		m_Context = scene;
	}

	void ScenePanel::OnImGuiRender()
	{
		ImGui::Begin("Scene");

		for (const Entity& entity : m_Context->GetAllEntities())
		{
			DrawEntityNode(entity);
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
		auto& tc = entity.GetComponent<TagComponent>();

		ImGuiTreeNodeFlags flags = (m_SelectionContext == entity ? ImGuiTreeNodeFlags_OpenOnArrow : 0) | ImGuiTreeNodeFlags_Selected;

		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tc.Tag.c_str());
		if (ImGui::IsItemClicked())
		{
			m_SelectionContext = entity;
			PropetiesPanel::SetSelectedContext(m_SelectionContext);
		}

		if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(0))
		{
			m_SelectionContext = {};
			PropetiesPanel::SetSelectedContext(m_SelectionContext);
		}

		if (opened)
		{
			ImGui::TreePop();
		}
	}
}