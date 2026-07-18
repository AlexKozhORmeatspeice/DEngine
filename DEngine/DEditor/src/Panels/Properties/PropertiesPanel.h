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

	private:
		void DrawComponents(const Entity& entity);

		void DrawTag(const Entity& entity);
		void DrawTransform(const Entity& entity);
		static Entity m_SelectedEntity;
	};
}
