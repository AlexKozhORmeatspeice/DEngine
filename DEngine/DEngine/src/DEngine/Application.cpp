#include "dpch.h"
#include "Application.h"
#include "DLog.h"

#include "Input/Input.h"

#include "DEngine/Renderer/RenderCommand.h"
#include "DEngine/Renderer/Renderer.h"

#include "DEngine/Renderer/Cameras/OrthCamera.h"

//TODO: TEMP
#include "GLFW/glfw3.h" 
#include "glad/glad.h"

namespace DEngine
{
    Application* Application::s_Instance = nullptr;

    Application::Application()
    {
        D_CORE_ASSERT(!s_Instance, "Application already exist");

        s_Instance = this;

        //Create window
        m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

        //Init ImGui
        m_ImGuiLayer = new ImGuiLayer();
        PushOverlay(m_ImGuiLayer);
    }

    Application::~Application()
    {
        
    }

    void Application::OnEvent(Event& e)
    {
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClosed));

		for (auto it = m_layerStack.end(); it != m_layerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.m_Handled)
				break;
		}
    }

    void Application::Run()
    {
		//TODO: перенести в init рендера
        glEnable(GL_DEPTH_TEST); 
		glDepthFunc(GL_LESS);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
		//TODO--------------------------

        while (m_Running)
        {
			//TODO: перенести реализацию в какой-то class Time независимый от платформы
            float time = (float)glfwGetTime(); 
            Timestep timestep = time - m_LastFrameTime;
            m_LastFrameTime = time;

            for (Layer* layer : m_layerStack)
            {
				layer->OnUpdate(timestep);
            }

            m_ImGuiLayer->Begin();
            for (Layer* layer : m_layerStack)
				layer->OnImGuiRenderer();
            m_ImGuiLayer->End();

            m_Window->OnUpdate();
        }
    }
    
    void Application::PushLayer(Layer* layer)
    {
        m_layerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PushOverlay(Layer* overlay)
    {
        m_layerStack.PushOverlay(overlay);
        overlay->OnAttach();
    }

    bool Application::OnWindowClosed(WindowCloseEvent& e)
    {
		m_Running = false;
		return true;
    }
}