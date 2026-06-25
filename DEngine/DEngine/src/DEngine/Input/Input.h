#pragma once
#include "DEngine/Core.h"

namespace DEngine
{
	class D_API Input
	{
	public:
		static inline bool IsKeyPressed(int keycode) { return s_Instance->IsKeyPressedImpl(keycode); }
		static inline bool IsMouseButtonPressed(int btn) { return s_Instance->IsMouseButtonPressedImpl(btn); }
		static inline std::pair<float, float> GetMousePos() { return s_Instance->GetMousePosImpl(); };
		static inline float GetMouseX() { return s_Instance->GetMouseXImpl(); };
		static inline float GetMouseY() { return s_Instance->GetMouseYImpl(); }

	protected:
		virtual bool IsKeyPressedImpl(int keycode) = 0;
		virtual bool IsMouseButtonPressedImpl(int btn) = 0;
		virtual inline std::pair<float, float> GetMousePosImpl() = 0;
		virtual inline float GetMouseXImpl() = 0;
		virtual inline float GetMouseYImpl() = 0;
	private:
		static Input* s_Instance;
	};
}
