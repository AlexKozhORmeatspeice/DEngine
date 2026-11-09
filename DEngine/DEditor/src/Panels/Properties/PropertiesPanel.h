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

		Entity& getSelectedEntity();

		void SetContext(const Ref<Scene>& scene);

	private:
		void DrawComponents(Entity& entity);

		void DrawTag(Entity& entity);
		void DrawTransform(Entity& entity);
		void DrawMeshRenderer(Entity& entity);
		void DrawCamera(Entity& entity);
		void DrawRigidbody(Entity& entity);
		void DrawCollider(Entity& entity);
		void DrawAdditionalData(Entity& entity);

		void DrawAssetSlot(const char* label, AssetHandle& slot, AssetType accepted, bool allowPrimitives);

		Ref<Scene> m_Context;
		static Entity m_SelectedEntity;
	};
}
