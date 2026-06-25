#pragma once

#include "DEngine/Input/Input.h"

namespace DEngine
{
	class WindowsInput : public Input
	{
	protected:
		virtual bool IsKeyPressedImpl(int keycode) override;
		virtual bool IsMouseButtonPressedImpl(int btn) override;
		virtual inline float GetMouseXImpl() override;
		virtual inline float GetMouseYImpl() override;

		virtual inline std::pair<float, float> GetMousePosImpl() override;
	};
}
