#include "dpch.h"
#include "Camera.h"

#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtc/quaternion.hpp"

namespace DEngine
{
	glm::vec3& Camera::GetForwardDir()
	{
		glm::quat rotation = glm::quat(glm::vec3(
			glm::radians(m_Rot.x),
			glm::radians(m_Rot.y),
			glm::radians(m_Rot.z)
		));

		return rotation * glm::vec3(0.0f, 0.0f, -1.0f);
	}

	glm::vec3& Camera::GetRightDir()
	{
		glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
		glm::quat rotX = glm::angleAxis(glm::radians(m_Rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::quat rotY = glm::angleAxis(glm::radians(m_Rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::quat rotZ = glm::angleAxis(glm::radians(m_Rot.z), glm::vec3(0.0f, 0.0f, 1.0f));

		glm::quat rotation = rotX * rotY * rotZ;

		right = rotation * right;
		return right;
	}

	void Camera::ChangeSize(uint32_t widht, uint32_t height)
	{
		
	}
}