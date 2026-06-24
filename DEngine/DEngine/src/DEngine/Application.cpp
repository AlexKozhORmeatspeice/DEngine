#include "dpch.h"
#include "Application.h"
#include "DLog.h"

namespace DEngine
{
#define BIND_EVENT_FN(X) std::bind(&X, this, std::placeholders::_1)

    Application::Application()
    {
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

        D_CORE_TRACE("{0}", e.ToString());
    }

    bool Application::OnWindowClosed(WindowCloseEvent& e)
    {
		m_Running = false;
		return true;
    }

    void Application::Run()
    {
        while (m_Running)
        {
            m_Window->OnUpdate();
        }
    }
}