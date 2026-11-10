#include "ScenePanel.h"

#include "Panels/Properties/PropertiesPanel.h"
#include "Panels/AssetDragDrop.h"
#include "DEngine/Profiler/Profiler.h"

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
		m_Children.clear();
		m_Roots.clear();
	}

	void ScenePanel::BuildHierarchy()
	{
		for (auto& entry : m_Children)
			entry.second.clear();
		m_Roots.clear();

		auto view = m_Context->View<TagComponent>();
		auto& registry = m_Context->GetRegistry();
		m_Roots.reserve(view.size());

		for (auto [handle, tag] : view.each())
		{
			(void)tag;
			const TransformComponent* transform = registry.try_get<TransformComponent>(handle);
			uint64_t parent = transform ? transform->Parent : 0;
			if (parent != 0 && m_Context->Contains(parent))
				m_Children[parent].push_back(handle);
			else
				m_Roots.push_back(handle);
		}

		for (auto it = m_Children.begin(); it != m_Children.end(); )
		{
			if (it->second.empty())
				it = m_Children.erase(it);
			else
				++it;
		}
	}

	void ScenePanel::OnRender()
	{
		D_PROFILE_FUNCTION();
		const bool visible = ImGui::Begin("Scene");

		if (visible && m_Context)
		{
			BuildHierarchy();
			for (entt::entity root : m_Roots)
				DrawEntityNode(root);
		}

		if (visible && m_Context)
		{
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
					m_Context->CreateEntity();
				ImGui::EndPopup();
			}

			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem("Delete entity"))
					m_Context->DestroyEntity(m_SelectionContext);
				ImGui::EndPopup();
			}
		}

		ImGui::End();
	}

	void ScenePanel::DrawEntityNode(entt::entity handle)
	{
		if (!m_Context || !m_Context->GetRegistry().valid(handle))
			return;

		const TagComponent& tag = m_Context->GetRegistry().get<TagComponent>(handle);
		const uint64_t id = m_Context->GetUUID(handle);
		const auto children = m_Children.find(id);
		const bool hasChildren = id != 0 && children != m_Children.end();

		Entity entity = m_Context->GetEntity(handle);

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		if (m_SelectionContext == entity)
			flags |= ImGuiTreeNodeFlags_Selected;
		if (!hasChildren)
			flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

		const bool inactive = !m_Context->IsActive(entity);
		if (inactive)
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.55f, 0.55f, 0.55f, 1.0f));

		const bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)handle, flags, "%s", tag.Tag.c_str());
		if (ImGui::IsItemClicked())
			SetSelected(entity);

		if (inactive)
			ImGui::PopStyleColor();

		if (opened && hasChildren)
		{
			for (entt::entity child : children->second)
				DrawEntityNode(child);
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
