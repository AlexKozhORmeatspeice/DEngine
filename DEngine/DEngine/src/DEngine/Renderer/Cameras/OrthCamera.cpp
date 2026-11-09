#include "dpch.h"
#include "OrthCamera.h"

#include "glm/gtc/matrix_transform.hpp"

namespace DEngine
{
	OrthographicCamera::OrthographicCamera()
		: Camera()
	{
		z_Near = 0.1f;
		z_Far = 3000.0f;
		RecalcProj();
		RecalcViewMat();
	}

	OrthographicCamera::OrthographicCamera(float size, float aspect, float nearClip, float farClip)
		: Camera(), m_Size(size), m_Aspect(aspect)
	{
		z_Near = nearClip;
		z_Far = farClip;
		RecalcProj();
		RecalcViewMat();
	}

	void OrthographicCamera::SetSize(float size)
	{
		m_Size = size < 0.01f ? 0.01f : size;
		RecalcProj();
	}

	void OrthographicCamera::ChangeSize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0)
			return;

		m_Aspect = (float)width / (float)height;
		RecalcProj();
		RecalcViewMat();
	}

	void OrthographicCamera::RecalcProj()
	{
		if (m_Aspect <= 0.0f)
			m_Aspect = 1.0f;

		float halfHeight = m_Size;
		float halfWidth = m_Size * m_Aspect;
		m_ProjMat = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, z_Near, z_Far);
		m_ViewProjMat = m_ProjMat * m_ViewMat;
	}
}
