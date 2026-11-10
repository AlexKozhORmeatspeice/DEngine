#pragma once

#include "imgui/imgui.h"
#include "DEngine.h"
#include "DEngine/Core.h"

#include <unordered_map>
#include <vector>

namespace DEngine
{
	//На все приложение будет только 1 окно с сценой
	class ScenePanel
	{
	public:
		ScenePanel() = default;
		ScenePanel(const Ref<Scene>& scene);

		void SetContext(const Ref<Scene>& scene);

		void OnRender();
		void SetSelected(Entity entity);
	private:
		void BuildHierarchy();
		void DrawEntityNode(entt::entity entity);

		Ref<Scene> m_Context;
		Entity m_SelectionContext;
		std::unordered_map<uint64_t, std::vector<entt::entity>> m_Children;
		std::vector<entt::entity> m_Roots;
	};
}
