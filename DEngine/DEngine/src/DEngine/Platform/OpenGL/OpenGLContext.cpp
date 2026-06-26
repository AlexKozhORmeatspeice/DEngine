#include "dpch.h"
#include "OpenGLContext.h"

#include "GLFW/glfw3.h"
#include "glad/glad.h"

namespace DEngine
{
	OpenGLContext::OpenGLContext(GLFWwindow* hWindow)
		: m_hWindow(hWindow)
	{
		D_CORE_ASSERT(hWindow, "Window handle is null");
	}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_hWindow);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		D_CORE_ASSERT(status, "Failed to initialize Glad");

		auto glStr = glGetString(GL_RENDERER);
		std::string gpuName(reinterpret_cast<const char*>(glStr));

		glStr = glGetString(GL_VERSION);
		std::string verName(reinterpret_cast<const char*>(glStr));

		D_CORE_INFO("Init OpenGL Renderer: {0}, {1}", verName, gpuName);
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_hWindow);
	}
}