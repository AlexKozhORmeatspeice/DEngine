#pragma once

#include "DEngine/Core.h"
#include "DEngine/Renderer/GraphicsContext.h"

class GLFWwindow;

namespace DEngine
{
	class D_API OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* hWindow);

		virtual void Init() override;
		virtual void SwapBuffers() override;
	private:
		GLFWwindow* m_hWindow;
	};
}
