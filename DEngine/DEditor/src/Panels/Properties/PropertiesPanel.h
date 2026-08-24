#pragma once

#include "imgui/imgui.h"
#include "DEngine.h"
#include "DEngine/Core.h"


namespace DEngine
{
	//На все приложение будет только 1 окно с свойствами
	class PropetiesPanel
	{
	public:
		void OnImGuiRender();

		static void SetSelectedContext(Entity& entity) { m_SelectedEntity = entity; }
		void SetContext(const Ref<Scene>& scene);

	private:
		void DrawComponents(Entity& entity);

		void DrawTag(const Entity& entity);
		void DrawTransform(Entity& entity);
		void DrawRigidbody(Entity& entity);
		void DrawCollider(Entity& entity);
		void DrawAdditionalData(Entity& entity);

		Ref<Scene> m_Context;
		static Entity m_SelectedEntity;
	};
}
