#pragma once

#include "Core.h"
#include "Window.h"
#include "SwapChain/SwapChain.h"
#include "DEngine/Renderer/Device/RenderDevice.h"
#include "Layer/LayerStack.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "ImGui/ImGuiLayer.h"

#include "DEngine/Renderer/Cameras/Camera.h"

#include "DEngine/Core/Timestep.h"

#include "queue"
#include "memory"

namespace DEngine
{
    class D_API Application
    {
    public:
        Application(const std::string& name = "DEngine App");
        virtual ~Application();

        void Run();

        void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

        void Close();

        ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		inline static Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window; }
		inline RenderDevice& GetDevice() { return *m_RenderDevice; }

        inline float GetFPS() const { return m_FPS; }

    private:

        void Init();
        void OnUpdate(const Timestep& ts);
        void OnRender(const Timestep& ts);
        void GUIUpdate();
        void Shutdown();

        void OnEvent(Event& e);
        bool OnWindowClosed(WindowCloseEvent& e);
        bool OnWindowResize(WindowResizeEvent& e);

    private:
		Scope<Window> m_Window;
        Scope<SwapChain> m_SwapChain;
        Scope<RenderDevice> m_RenderDevice;

		ImGuiLayer* m_ImGuiLayer;
        LayerStack m_layerStack;

        bool m_Running = true;
        bool m_Minimized = false;

        float m_LastFrameTime = 0.0f;
        int m_FrameCount = 0;
		float m_FPSTimer = 0.0f;
		float m_FPS = 0.0f;

        static Application* s_Instance;
    };

    //To be defined in client
	Application* CreateApplication();
}