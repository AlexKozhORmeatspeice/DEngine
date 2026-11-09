#include "dpch.h"
#include "DEngine/Renderer/Cameras/PerspCamera.h"

#include "glm/gtc/matrix_transform.hpp"

namespace DEngine
{
	PerspectiveCamera::PerspectiveCamera()
		: Camera()
	{
		z_Near = 0.1f;
		z_Far = 3000.0f;
		m_Pos = glm::vec3(0.0f);
		m_Rot = glm::vec3(0.0f);
		RecalcProj();
		RecalcViewMat();
	}

	PerspectiveCamera::PerspectiveCamera(float fov, uint32_t width, uint32_t height, float _near, float _far, float _zoomLevel)
		: Camera(), m_Fov(glm::radians(fov)), m_Width(width), m_Height(height), m_Zoom(_zoomLevel)
	{
		z_Near = _near;
		z_Far = _far;
		m_Pos = glm::vec3(0.0f);
		m_Rot = glm::vec3(0.0f);
		RecalcProj();
		RecalcViewMat();
	}

	void PerspectiveCamera::SetVerticalFov(float fovDegrees)
	{
		m_Fov = glm::radians(glm::clamp(fovDegrees, 1.0f, 179.0f));
		RecalcProj();
	}

	void PerspectiveCamera::ChangeSize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0)
			return;

		m_Width = width;
		m_Height = height;
		RecalcProj();
		RecalcViewMat();
	}

	void PerspectiveCamera::RecalcProj()
	{
		if (m_Width == 0 || m_Height == 0)
			return;

		m_ProjMat = glm::perspectiveFov(m_Fov, (float)m_Width * m_Zoom, (float)m_Height * m_Zoom, z_Near, z_Far);
		m_ViewProjMat = m_ProjMat * m_ViewMat;
	}
}
