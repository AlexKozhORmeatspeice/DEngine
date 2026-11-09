#pragma once

#include "glm/glm.hpp"
#include "Camera.h"

namespace DEngine
{
	class PerspectiveCamera : public Camera
	{
	public:
		PerspectiveCamera();
		PerspectiveCamera(float fov, uint32_t width, uint32_t height, float _near = 0.1f, float _far = 3000.0f, float _zoomLevel = 0.1f);

		virtual void ChangeSize(uint32_t width, uint32_t height) override;
		virtual bool IsOrthographic() const override { return false; }

		void SetVerticalFov(float fovDegrees);
		float GetVerticalFov() const { return glm::degrees(m_Fov); }

	protected:
		virtual void RecalcProj() override;

	private:
		float m_Fov = glm::radians(60.0f);
		uint32_t m_Width = 1920;
		uint32_t m_Height = 1080;
		float m_Zoom = 0.1f;
	};
}
