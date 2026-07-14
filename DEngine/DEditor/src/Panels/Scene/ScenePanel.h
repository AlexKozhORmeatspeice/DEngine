#pragma once

#include "imgui/imgui.h"
#include "DEngine.h"
#include "DEngine/Core.h"

namespace DEngine
{
	//На все приложение будет только 1 окно с сценой
	class ScenePanel
	{
	public:
		ScenePanel() = default;
		ScenePanel(const Ref<Scene>& scene);

		void SetContext(const Ref<Scene>& scene);

		void OnImGuiRender();
	private:
		void DrawEntityNode(const Entity& entity);

		Ref<Scene> m_Context;
		Entity m_SelectionContext;
	};
}
