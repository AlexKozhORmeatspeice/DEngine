#pragma once

#include "DEngine/Core.h"

namespace DEngine
{
	class Window;

	class D_API RenderDevice
	{
	public:
		virtual ~RenderDevice() = default;

		static Scope<RenderDevice> Create(Window& window);

		virtual void* GetNativeDevice() const = 0;
	};
}
