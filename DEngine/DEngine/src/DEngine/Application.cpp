#include "dpch.h"
#include "Application.h"
#include "DLog.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "Input/Input.h"

namespace DEngine
{
    Application* Application::s_Instance = nullptr;
    static GLenum ShaderDataTypeToOpenGLType(ShaderDataType type)
    {
        switch (type)
        {
			case DEngine::ShaderDataType::None:   return GL_FLOAT;
			case DEngine::ShaderDataType::Float:  return GL_FLOAT;
			case DEngine::ShaderDataType::Float2: return GL_FLOAT;
			case DEngine::ShaderDataType::Float3: return GL_FLOAT;
			case DEngine::ShaderDataType::Float4: return GL_FLOAT;
			case DEngine::ShaderDataType::Mat3:   return GL_FLOAT;
			case DEngine::ShaderDataType::Mat4:   return GL_FLOAT;
			case DEngine::ShaderDataType::Int:    return GL_INT;
			case DEngine::ShaderDataType::Int2:   return GL_INT;
			case DEngine::ShaderDataType::Int3:   return GL_INT;
			case DEngine::ShaderDataType::Int4:   return GL_INT;
			case DEngine::ShaderDataType::Bool:   return GL_BOOL;
        }

        D_CORE_ASSERT(false, "Can't convert ShaderDataType to OpenGL type");
        return 0;
    }

    Application::Application()
    {
        D_CORE_ASSERT(!s_Instance, "Application already exist");

        s_Instance = this;

        m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

        m_ImGuiLayer = new ImGuiLayer();
        PushOverlay(m_ImGuiLayer);

        glGenVertexArrays(1, &m_VertexArray);
        glBindVertexArray(m_VertexArray);

        float verts[7 * 3]
        {
            -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
            0.0f,   0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f
        };
        m_VertexBuffer.reset(VertexBuffer::Create(verts, sizeof(verts)));

        BufferLayout layout =
        {
            {ShaderDataType::Float3, "a_Position"},
            {ShaderDataType::Float4, "a_Color"},
        };

        uint32_t ind = 0;

        m_VertexBuffer->SetLayout(layout);

        for (const auto& elem : m_VertexBuffer->GetLayout())
        {
			glEnableVertexAttribArray(ind);
			glVertexAttribPointer(
                ind, 
                elem.GetComponentCount(), 
                ShaderDataTypeToOpenGLType(elem.Type), 
                elem.IsNorm ? GL_TRUE : GL_FALSE, 
                layout.GetStride(),
                (const void*)elem.Offset);

			ind++;
        }

        unsigned int inds[3] = { 0, 1, 2 };
        m_IndexBuffer.reset(IndexBuffer::Create(inds, sizeof(inds) / sizeof(uint32_t)));

        std::string vertSrc = R"(
            #version 330 core

            layout(location = 0) in vec3 a_Position;
            layout(location = 1) in vec4 a_Color;

            out vec3 v_Pos;
            out vec4 v_Color;
            
            void main()
			{
                v_Pos = a_Position;
                v_Color = a_Color;
                gl_Position = vec4(a_Position, 1.0f);
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
            glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            m_Shader->Bind();

            glBindVertexArray(m_VertexArray);
            glDrawElements(GL_TRIANGLES, m_IndexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);

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