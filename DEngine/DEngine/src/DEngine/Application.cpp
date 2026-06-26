#include "dpch.h"
#include "Application.h"
#include "DLog.h"

#include "Input/Input.h"

#include "DEngine/Renderer/RenderCommand.h"
#include "DEngine/Renderer/Renderer.h"

#include "DEngine/Renderer/Cameras/OrthCamera.h"

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

        //Set Renderer
        ///Set camera
        m_Camera = std::make_shared<OrthographicCamera>(-1.0f, 1.0f, -1.0f, 1.0f);

        ///Init buffers
        m_VertexArray.reset(VertexArray::Create());

        float verts[8 * 7] = {
			-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 0.5f, 0.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f 
		};
        m_VertexBuffer.reset(VertexBuffer::Create(verts, sizeof(verts)));
        BufferLayout layout =
        {
            {ShaderDataType::Float3, "a_Position"},
            {ShaderDataType::Float4, "a_Color"},
        };
        m_VertexBuffer->SetLayout(layout);
        m_VertexArray->AddVertexBuffer(m_VertexBuffer);

        unsigned int inds[36] = {
			0, 1, 2,
			2, 3, 0,
			4, 6, 5,
			4, 7, 6,
			0, 7, 3,
			0, 4, 7,
			1, 5, 6,
			1, 6, 2,
			3, 2, 6,
			3, 6, 7,
			0, 5, 1,
			0, 4, 5
		};
        m_IndexBuffer.reset(IndexBuffer::Create(inds, sizeof(inds) / sizeof(uint32_t)));
        m_VertexArray->SetIndexBuffer(m_IndexBuffer);

        ///Init shaders
        std::string vertSrc = R"(
            #version 330 core

            layout(location = 0) in vec3 a_Position;
            layout(location = 1) in vec4 a_Color;

            uniform mat4 u_ViewProj;

            out vec3 v_Pos;
            out vec4 v_Color;
            
            void main()
			{
                v_Pos = a_Position;
                v_Color = a_Color;
                gl_Position = u_ViewProj * vec4(a_Position, 1.0f);
			}
        )";

        std::string fragSrc = R"(
            #version 330 core

            layout(location = 0) out vec4 color;

            in vec3 v_Pos;
            in vec4 v_Color;
            
            void main()
			{
                color = vec4(v_Color);
			}
        )";

        m_Shader.reset(Shader::Create(vertSrc, fragSrc));
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
            m_Camera->SetRot({ 0.0f, 0.0f, 45.0f });
            RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.15f, 1.0f });
            RenderCommand::Clear();

            Renderer::BeginScene(m_Camera);

            Renderer::Submit(m_VertexArray, m_Shader);

            Renderer::EndScene();

            for (Layer* layer : m_layerStack)
            {
				layer->OnUpdate();
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