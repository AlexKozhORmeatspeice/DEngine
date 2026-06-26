#include "dpch.h"
#include "OrthCamera.h"

#include "glm/gtc/matrix_transform.hpp"

namespace DEngine
{
	OrthographicCamera::OrthographicCamera()
		: Camera()
	{
		z_Near = 0.1f;
		z_Far = 100.0f;

		m_ProjMat = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, z_Near, z_Far);
		m_ViewMat = glm::mat4(1.0f);
		m_ViewProjMat = m_ProjMat * m_ViewMat;
	}

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top, float z_near, float z_far)
		: Camera()
	{
		z_Near = z_near;
		z_Far = z_far;

		m_ProjMat = glm::ortho(left, right, bottom, top, z_Near, z_Far);
		m_ViewMat = glm::mat4(1.0f);
		m_ViewProjMat = m_ProjMat * m_ViewMat;
	}

	void OrthographicCamera::RecalcViewMat()
	{
		glm::mat4 trans = glm::mat4(1.0f);
		trans = glm::translate(trans, m_Pos);
		trans = glm::rotate(trans, glm::radians(m_Rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
		trans = glm::rotate(trans, glm::radians(m_Rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
		trans = glm::rotate(trans, glm::radians(m_Rot.z), glm::vec3(0.0f, 0.0f, 1.0f));

		m_ViewMat = glm::inverse(trans);
		m_ViewProjMat = m_ProjMat * m_ViewMat;
	}
}
