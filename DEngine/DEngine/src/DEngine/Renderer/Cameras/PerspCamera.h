#pragma once

#include "glm/glm.hpp"
#include "Camera.h"

namespace DEngine
{
	class PerspectiveCamera : public Camera
	{
	public:
		PerspectiveCamera();
		PerspectiveCamera(float fov, uint32_t width, uint32_t height, float _near = 0.01f, float _far = 1000.0f, float _zoomLevel = 0.1f);

		virtual void ChangeSize(uint32_t width, uint32_t height) override;
	protected:
		virtual void RecalcViewMat() override;

	private:
		float m_Fov = 60.0f;
		uint32_t m_Width = 1920;
		uint32_t m_Height = 1080;

		float m_Zoom = 0.1f;
	};
}
