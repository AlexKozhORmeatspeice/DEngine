#pragma once

#include "glm/glm.hpp"
#include "Camera.h"

//TODO: обновить логику для совместимости с Camera
namespace DEngine
{
	class OrthographicCamera : public Camera
	{
	public:
		OrthographicCamera();
		OrthographicCamera(float left, float right, float bottom, float top, float near = 0.1f, float far = 100.0f);

	protected:
		virtual void RecalcViewMat() override;
	};
}
