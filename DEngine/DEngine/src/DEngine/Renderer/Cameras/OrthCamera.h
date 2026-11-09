#pragma once

#include "glm/glm.hpp"
#include "Camera.h"

namespace DEngine
{
	class OrthographicCamera : public Camera
	{
	public:
		OrthographicCamera();
		OrthographicCamera(float size, float aspect, float nearClip = 0.1f, float farClip = 3000.0f);

		virtual void ChangeSize(uint32_t width, uint32_t height) override;
		virtual bool IsOrthographic() const override { return true; }

		void SetSize(float size);
		float GetSize() const { return m_Size; }

	protected:
		virtual void RecalcProj() override;

	private:
		float m_Size = 10.0f;
		float m_Aspect = 16.0f / 9.0f;
	};
}
