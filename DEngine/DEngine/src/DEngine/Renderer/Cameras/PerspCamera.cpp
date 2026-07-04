#include "dpch.h"
#include "DEngine/Renderer/Cameras/PerspCamera.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtc/quaternion.hpp"

namespace DEngine
{
	PerspectiveCamera::PerspectiveCamera()
		: Camera(), m_Fov(glm::radians(60.0f)), m_Width(1920), m_Height(1080), m_Zoom(0.1f)
	{
		z_Near = 0.1f;
		z_Far = 1000.0f;
		m_Pos = glm::vec3(0.0f);
		m_Rot = glm::vec3(0.0f);

		m_ProjMat = glm::perspectiveFov(m_Fov, m_Width * m_Zoom, m_Height * m_Zoom, z_Near, z_Far);

		m_ViewMat = glm::mat4(1.0f);
		m_ViewProjMat = m_ProjMat * m_ViewMat;
	}

	PerspectiveCamera::PerspectiveCamera(float fov, uint32_t width, uint32_t height, float _near, float _far, float _zoomLevel)
		: Camera(), m_Fov(glm::radians(fov)), m_Width(width), m_Height(height), m_Zoom(_zoomLevel)
	{
		z_Near = _near;
		z_Far = _far;
		m_Pos = glm::vec3(0.0f);
		m_Rot = glm::vec3(0.0f);

		m_ProjMat = glm::perspectiveFov(m_Fov, m_Width * m_Zoom, m_Height * m_Zoom, z_Near, z_Far);

		m_ViewMat = glm::mat4(1.0f);
		m_ViewProjMat = m_ProjMat * m_ViewMat;
	}

	void PerspectiveCamera::ChangeSize(uint32_t width, uint32_t height)
	{
		m_Width = width;
		m_Height = height;

		m_ProjMat = glm::perspectiveFov(m_Fov, m_Width * m_Zoom, m_Height * m_Zoom, z_Near, z_Far);

		RecalcViewMat();
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
