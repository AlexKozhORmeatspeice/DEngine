#include "PropertiesPanel.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace DEngine
{
	Entity PropetiesPanel::m_SelectedEntity = {};

	void PropetiesPanel::OnImGuiRender()
	{
		ImGui::Begin("Properties");

		if (m_SelectedEntity)
		{
			DrawComponents(m_SelectedEntity);
		}

		ImGui::End();
	}

	void PropetiesPanel::DrawComponents(const Entity& entity)
	{
		DrawTag(entity);
		DrawTransform(entity);
	}

	void PropetiesPanel::DrawTag(const Entity& entity)
	{
		if (!entity.HasComponent<TagComponent>()) return;

		TagComponent tagComp = entity.GetComponent<TagComponent>();
		
		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		strcpy_s(buffer, sizeof(buffer), tagComp.Tag.c_str());

		if (ImGui::InputText("Tag", buffer, sizeof(buffer)))
		{
			tagComp.Tag = std::string(buffer);
		}
	}
	
	void PropetiesPanel::DrawTransform(const Entity& entity)
	{
		if (!entity.HasComponent<TransformComponent>()) return;
		if (!ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform")) return;

		TransformComponent transComp = entity.GetComponent<TransformComponent>();

		// Position
		glm::vec3 position = transComp.GetPosition();
		if (ImGui::DragFloat3("Position", glm::value_ptr(position), 0.1f))
		{
			transComp.SetPosition(position);
		}

		// Rotation
		glm::vec3 euler = transComp.GetRotationEuler();
		if (ImGui::DragFloat3("Rotation", glm::value_ptr(euler), 0.1f))
		{
			for (int i = 0; i < 3; i++)
			{
				euler[i] = fmod(euler[i], 360.0f);
				if (euler[i] < 0.0f)
					euler[i] += 360.0f;
			}
			transComp.SetRotationEuler(euler);
		}

		// Scale
		glm::vec3 scale = transComp.GetScale();
		if (ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.1f))
		{
			transComp.SetScale(scale);
		}

		ImGui::TreePop();
	}
}