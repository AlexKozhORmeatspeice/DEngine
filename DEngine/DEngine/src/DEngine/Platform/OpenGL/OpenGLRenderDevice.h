#pragma once

#include "DEngine/Renderer/Device/RenderDevice.h"

namespace DEngine
{
	class Window;

	class D_API OpenGLRenderDevice : public RenderDevice
	{
	public:
		OpenGLRenderDevice(Window& window);
		~OpenGLRenderDevice() override = default;

		virtual void* GetNativeDevice() const { return nullptr; }
	private:
		Window& m_Window;
	};
}
