#pragma once

#include "DEngine/Core.h"
#define GLFW_INCLUDE_VULKAN
#include "DEngine/Renderer/GraphicsContext.h"

class GLFWwindow;

namespace DEngine
{
	class D_API VulkanContext : public GraphicsContext
	{
	public:
		VulkanContext(GLFWwindow* hWindow);

		virtual void Init() override;
		virtual void SwapBuffers() override;
	private:
		GLFWwindow* m_hWindow;
	};
}
