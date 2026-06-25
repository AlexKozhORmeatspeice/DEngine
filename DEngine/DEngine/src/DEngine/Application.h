#pragma once

#include "Core.h"
#include "Window.h"
#include "Layer/LayerStack.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "ImGui/ImGuiLayer.h"

namespace DEngine
{
    class D_API Application
    {
    public:
        Application();
        virtual ~Application();

        void Run();

        void OnEvent(Event& e);

        void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		inline static Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window; }
    private:
        bool OnWindowClosed(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
        bool m_Running = true;
        LayerStack m_layerStack;

        unsigned int m_VertexArray, m_VertexBuffer, m_IndexBuffer;

        static Application* s_Instance;
    };

    //To be defined in client
	Application* CreateApplication();
}