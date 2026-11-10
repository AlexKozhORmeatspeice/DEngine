#pragma once

#include "DEngine.h"
#include "imgui/imgui.h"
#include "ImGuizmo.h"

namespace DEngine
{
	class PropetiesPanel;

	class EditorViewportPanel
	{
	public:
		void Init(const Ref<PerspectiveCamera>& camera);
		void OnUpdate(const Timestep& ts);
		void OnRender(const Ref<Scene>& scene, PropetiesPanel& properties);
		void RenderGraphic(const Timestep& ts, const Ref<Scene>& scene);

		bool IsOpen() const { return m_Open; }
		bool IsFocused() const { return m_Focused; }
		bool IsHovered() const { return m_Hovered; }

	private:
		void DrawGizmo(PropetiesPanel& properties);

		Ref<PerspectiveCamera> m_Camera;
		Ref<Framebuffer> m_Framebuffer;
		glm::vec2 m_Size{ 0.0f, 0.0f };
		bool m_Open = false;
		bool m_Focused = false;
		bool m_Hovered = false;
		bool m_TabJustPressed = false;
		ImGuizmo::OPERATION m_GuizmoType = ImGuizmo::OPERATION::TRANSLATE;
	};
}
