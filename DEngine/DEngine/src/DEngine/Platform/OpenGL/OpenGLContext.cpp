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

		auto vendorGPUName = glGetString(GL_RENDERER);
		std::string res1(reinterpret_cast<const char*>(vendorGPUName));

		auto versionName = glGetString(GL_VERSION);
		std::string res2(reinterpret_cast<const char*>(versionName));

		D_CORE_INFO("Init OpenGL Renderer: {0}, {1}", res2, res1);
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_hWindow);
	}
}