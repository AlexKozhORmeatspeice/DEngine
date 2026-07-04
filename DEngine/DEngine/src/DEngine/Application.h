#pragma once

#include "Core.h"
#include "Window.h"
#include "Layer/LayerStack.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "ImGui/ImGuiLayer.h"

#include "DEngine/Renderer/Cameras/Camera.h"

#include "DEngine/Core/Timestep.h"

//TODO: TEMP
#include "DEngine/Renderer/Buffers/Buffer.h"
#include "DEngine/Renderer/Shader/Shader.h"
#include "DEngine/Renderer/Buffers/VertexArray.h"

namespace DEngine
{
    class D_API Application
    {
    public:
        Application(const std::string& name = "Hazel App");
        virtual ~Application();

        void Run();

        void OnEvent(Event& e);

        void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

        void Close();

		inline static Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window; }
    private:
        bool OnWindowClosed(WindowCloseEvent& e);
        bool OnWindowResize(WindowResizeEvent& e);

    private:
		Scope<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
        LayerStack m_layerStack;

        bool m_Running = true;
        bool m_Minimized = false;

        float m_LastFrameTime = 0.0f;

        static Application* s_Instance;
    };

    //To be defined in client
	Application* CreateApplication();
}