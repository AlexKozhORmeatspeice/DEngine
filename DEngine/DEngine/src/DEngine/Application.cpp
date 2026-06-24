#include "dpch.h"
#include "Application.h"
#include "DLog.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace DEngine
{
    
    Application* Application::s_Instance = nullptr;

    Application::Application()
    {
        D_CORE_ASSERT(!s_Instance, "Application already exist");

        s_Instance = this;

        m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
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
        while (m_Running)
        {
            glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            for (Layer* layer : m_layerStack)
            {
				layer->OnUpdate();
            }

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