#include "dpch.h"
#include "DEngine/Renderer/Cameras/PerspCamera.h"

#include "glm/gtc/matrix_transform.hpp"

namespace DEngine
{
	PerspectiveCamera::PerspectiveCamera()
		: Camera(), m_Fov(60.0f), m_Width(1920), m_Height(1080)
	{
		z_Near = 0.1f;
		z_Far = 100.0f;
		m_Pos = glm::vec3(0.0f);
		m_Rot = glm::vec3(0.0f);

		m_ProjMat = glm::perspectiveFov(glm::radians(m_Fov), m_Width, m_Height, z_Near, z_Far);

		m_ViewMat = glm::mat4(1.0f);
		m_ViewProjMat = m_ProjMat * m_ViewMat;
	}

	PerspectiveCamera::PerspectiveCamera(float fov, float width, float height, float _near, float _far)
		: Camera(), m_Fov(fov), m_Width(width), m_Height(height)
	{
		z_Near = _near;
		z_Far = _far;
		m_Pos = glm::vec3(0.0f);
		m_Rot = glm::vec3(0.0f);

		m_ProjMat = glm::perspectiveFov(glm::radians(m_Fov), m_Width, m_Height, z_Near, z_Far);

		m_ViewMat = glm::mat4(1.0f);
		m_ViewProjMat = m_ProjMat * m_ViewMat;
	}

	void PerspectiveCamera::RecalcViewMat()
	{
		glm::mat4 trans = glm::mat4(1.0f);
		trans = glm::translate(trans, m_Pos);

		glm::quat rotation = glm::quat(glm::vec3(
			glm::radians(m_Rot.x),
			glm::radians(m_Rot.y),
			glm::radians(m_Rot.z)
		));

		trans = trans * glm::mat4_cast(rotation);

		m_ViewMat = glm::inverse(trans);
		m_ViewProjMat = m_ProjMat * m_ViewMat;
	}
}
