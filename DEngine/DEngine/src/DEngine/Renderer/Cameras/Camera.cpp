#include "dpch.h"
#include "Camera.h"

#include "glm/gtc/matrix_transform.hpp"

namespace DEngine
{
	void Camera::SetRot(const glm::vec3& rot)
	{
		m_Rot = rot;
		m_Orientation = glm::quat(glm::radians(m_Rot));
		RecalcViewMat();
	}

	void Camera::SetOrientation(const glm::quat& orientation)
	{
		m_Orientation = glm::normalize(orientation);
		m_Rot = glm::degrees(glm::eulerAngles(m_Orientation));
		RecalcViewMat();
	}

	glm::vec3 Camera::GetForwardDir() const
	{
		return m_Orientation * glm::vec3(0.0f, 0.0f, -1.0f);
	}

	glm::vec3 Camera::GetRightDir() const
	{
		return m_Orientation * glm::vec3(1.0f, 0.0f, 0.0f);
	}

	glm::vec3 Camera::GetUpDir() const
	{
		return m_Orientation * glm::vec3(0.0f, 1.0f, 0.0f);
	}

	void Camera::SetClipPlanes(float nearClip, float farClip)
	{
		z_Near = nearClip;
		z_Far = farClip;
		RecalcProj();
	}

	void Camera::RecalcViewMat()
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Pos) * glm::mat4_cast(m_Orientation);
		m_ViewMat = glm::inverse(transform);
		m_ViewProjMat = m_ProjMat * m_ViewMat;
	}
}
