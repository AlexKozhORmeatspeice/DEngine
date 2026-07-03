#pragma once

#include "glm/glm.hpp"
#include "Camera.h"

namespace DEngine
{
	class PerspectiveCamera : public Camera
	{
	public:
		PerspectiveCamera();
		PerspectiveCamera(float fov, float width, float height, float _near = 0.01f, float _far = 1000.0f);

	protected:
		virtual void RecalcViewMat() override;

	private:
		float m_Fov = 60.0f;
		float m_Width = 1920.0f;
		float m_Height = 1080.0f;
	};
}
