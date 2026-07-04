#pragma once

#include "DEngine/Core.h"

#define GLM_ENABLE_EXPERIMENTAL

#include "glm/glm.hpp"

namespace DEngine
{
	class D_API Camera
	{
	public:
		const glm::vec3& GetPos() const { return m_Pos; }
		void SetPos(const glm::vec3& pos) 
		{ 
			m_Pos = pos;
			RecalcViewMat();
		}

		const glm::vec3& GetRot() const { return m_Rot; }
		void SetRot(const glm::vec3& rot) 
		{ 
			m_Rot = rot; 
			RecalcViewMat();
		}

		glm::vec3& GetForwardDir();
		glm::vec3& GetRightDir();

		virtual void ChangeSize(uint32_t widht, uint32_t height) = 0;

		const glm::mat4& GetProjMat() const { return m_ProjMat; }
		const glm::mat4& GetViewMat() const { return m_ViewMat; }
		const glm::mat4& GetViewProjMat() const { return m_ViewProjMat; }

	protected:
		virtual void RecalcViewMat() = 0;
		Camera() {}

		float z_Near = 0.1f;
		float z_Far = 100.0f;

		glm::mat4 m_ProjMat = glm::mat4(1.0f);
		glm::mat4 m_ViewMat = glm::mat4(1.0f);
		glm::mat4 m_ViewProjMat = glm::mat4(1.0f);

		glm::vec3 m_Pos = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_Rot = { 0.0f, 0.0f, 0.0f };
	};
}
