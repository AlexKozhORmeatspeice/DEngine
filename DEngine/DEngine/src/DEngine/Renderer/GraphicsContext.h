#pragma once
#include "DEngine/Core.h"

namespace DEngine
{
	class D_API GraphicsContext
	{
	public:
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
	};
}
