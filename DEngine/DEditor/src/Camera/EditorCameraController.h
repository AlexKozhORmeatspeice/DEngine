#pragma once

#include "DEngine.h"

namespace DEngine
{
	class EditorViewportPanel;

	class EditorCameraController
	{
	public:
		void Init(uint32_t width, uint32_t height);
		void SetViewport(EditorViewportPanel* viewport) { m_Viewport = viewport; }

		void OnUpdate(const Timestep& ts);
		bool OnMousePressed(MouseButtonPressedEvent& event);
		bool OnMouseReleased(MouseButtonReleasedEvent& event);
		bool OnMouseMoved(MouseMovedEvent& event);
		bool OnMouseScrolled(MouseScrolledEvent& event);

		const Ref<PerspectiveCamera>& GetCamera() const { return m_Camera; }

	private:
		bool CanControl() const;

		EditorViewportPanel* m_Viewport = nullptr;
		Ref<PerspectiveCamera> m_Camera;
		glm::vec3 m_Position{ 0.0f };
		glm::vec3 m_Rotation{ 0.0f };
		float m_Speed = 200.0f;
		float m_Sensitivity = 0.15f;
		float m_LastMouseX = 0.0f;
		float m_LastMouseY = 0.0f;
		bool m_Controlling = false;
		bool m_FirstMouseMove = true;
	};
}
