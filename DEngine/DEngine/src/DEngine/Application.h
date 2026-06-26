#pragma once

#include "Core.h"
#include "Window.h"
#include "Layer/LayerStack.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "ImGui/ImGuiLayer.h"

#include "DEngine/Renderer/Cameras/Camera.h"

//TODO: TEMP
#include "DEngine/Renderer/Buffers/Buffer.h"
#include "DEngine/Renderer/Shader/Shader.h"
#include "DEngine/Renderer/Buffers/VertexArray.h"

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

        std::shared_ptr<VertexArray> m_VertexArray;
        std::shared_ptr<VertexBuffer> m_VertexBuffer;
        std::shared_ptr<IndexBuffer> m_IndexBuffer;

        std::shared_ptr<Camera> m_Camera;

        std::shared_ptr<Shader> m_Shader;
        static Application* s_Instance;
    };

    //To be defined in client
	Application* CreateApplication();
}