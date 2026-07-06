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

    Application::Application(const std::string& name)
    {
        D_CORE_ASSERT(!s_Instance, "Application already exist");

        s_Instance = this;

        //Create window
        m_Window = Window::Create(WindowProps(name));
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

        //Renderer
        Renderer::Init();

        //Init ImGui
        m_ImGuiLayer = new ImGuiLayer();
        PushOverlay(m_ImGuiLayer);
    }

    Application::~Application()
    {
        
    }

    void Application::OnEvent(Event& event)
    {
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClosed));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

		for (auto it = m_layerStack.end(); it != m_layerStack.begin(); )
		{
			(*--it)->OnEvent(event);
			if ((event).m_Handled)
				break;
		}
    }

    void Application::Run()
    {
        Init();

        while (m_Running)
        {
			//TODO: перенести реализацию в какой-то class Time независимый от платформы
            //Обновляем время
            float time = (float)glfwGetTime(); 
            Timestep timestep = time - m_LastFrameTime;
            m_LastFrameTime = time;

            //Ставим FPS
            m_FrameCount++;
            if (time - m_FPSTimer >= 1.0f)
            {
                m_FPS = m_FrameCount;
                m_FrameCount = 0;
                m_FPSTimer = time;
            }

            if (!m_Minimized)
            {
				OnUpdate(timestep);
            }

            OnRender(timestep);

            GUIUpdate();

            m_Window->OnUpdate();
        }

        Shutdown();
    }

    void Application::Init()
    {
		for (Layer* layer : m_layerStack)
			layer->Init();
    }

    void Application::OnUpdate(const Timestep& ts)
    {
		for (Layer* layer : m_layerStack)
			layer->OnUpdate(ts);
    }

    void Application::OnRender(const Timestep& ts)
    {
		for (Layer* layer : m_layerStack)
			layer->OnRender(ts);
    }

    void Application::GUIUpdate()
    {
		m_ImGuiLayer->Begin();
		for (Layer* layer : m_layerStack)
			layer->OnImGuiRenderer();
		m_ImGuiLayer->End();
    }

    void Application::Shutdown()
    {
		for (Layer* layer : m_layerStack)
			layer->Shutdown();
    }
    
    void Application::PushLayer(Layer* layer)
    {
        m_layerStack.PushLayer(layer);
    }

    void Application::PushOverlay(Layer* overlay)
    {
        m_layerStack.PushOverlay(overlay);
    }

    void Application::Close()
    {
        m_Running = false;
    }

    bool Application::OnWindowClosed(WindowCloseEvent& e)
    {
		m_Running = false;
		return true;
    }

    bool Application::OnWindowResize(WindowResizeEvent& e)
    {
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

        m_Minimized = false;

        Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
        return false;
    }
}