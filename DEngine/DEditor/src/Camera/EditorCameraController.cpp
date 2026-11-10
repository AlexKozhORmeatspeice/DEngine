#include "EditorCameraController.h"

#include "Panels/Viewport/EditorViewportPanel.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace DEngine
{
	void EditorCameraController::Init(uint32_t width, uint32_t height)
	{
		m_Camera = std::make_shared<PerspectiveCamera>(60.0f, width, height);
		m_Camera->SetPos(m_Position);
		m_Camera->SetRot(m_Rotation);
	}

	bool EditorCameraController::CanControl() const
	{
		return m_Viewport && m_Viewport->IsFocused() && m_Viewport->IsHovered();
	}

	void EditorCameraController::OnUpdate(const Timestep& ts)
	{
		if (!m_Camera || !CanControl())
			return;

		const glm::vec3 forward = m_Camera->GetForwardDir();
		const glm::vec3 right = m_Camera->GetRightDir();
		const float step = m_Speed * ts.GetSeconds();

		if (Input::IsKeyPressed(D_KEY_W))
			m_Position += forward * step;
		if (Input::IsKeyPressed(D_KEY_S))
			m_Position -= forward * step;
		if (Input::IsKeyPressed(D_KEY_A))
			m_Position -= right * step;
		if (Input::IsKeyPressed(D_KEY_D))
			m_Position += right * step;

		m_Camera->SetPos(m_Position);
		m_Camera->SetRot(m_Rotation);
	}

	bool EditorCameraController::OnMousePressed(MouseButtonPressedEvent& event)
	{
		if (event.GetMouseButton() != D_MOUSE_BUTTON_RIGHT)
			return false;

		m_Controlling = true;
		m_FirstMouseMove = true;
		return true;
	}

	bool EditorCameraController::OnMouseReleased(MouseButtonReleasedEvent& event)
	{
		if (event.GetMouseButton() != D_MOUSE_BUTTON_RIGHT)
			return false;

		m_Controlling = false;
		m_FirstMouseMove = true;
		return true;
	}

	bool EditorCameraController::OnMouseMoved(MouseMovedEvent& event)
	{
		if (!CanControl() || !m_Controlling || !m_Camera)
			return false;

		const float x = event.GetX();
		const float y = event.GetY();
		if (m_FirstMouseMove)
		{
			m_LastMouseX = x;
			m_LastMouseY = y;
			m_FirstMouseMove = false;
			return false;
		}

		const float deltaX = x - m_LastMouseX;
		const float deltaY = y - m_LastMouseY;
		m_LastMouseX = x;
		m_LastMouseY = y;

		m_Rotation.y -= deltaX * m_Sensitivity;
		m_Rotation.x -= deltaY * m_Sensitivity;
		m_Rotation.x = glm::clamp(m_Rotation.x, -89.0f, 89.0f);
		m_Camera->SetRot(m_Rotation);
		return true;
	}

	bool EditorCameraController::OnMouseScrolled(MouseScrolledEvent& event)
	{
		if (!CanControl() || !m_Camera)
			return false;

		m_Position += m_Camera->GetForwardDir() * event.GetYOffset() * 20.0f;
		m_Camera->SetPos(m_Position);
		return true;
	}
}
