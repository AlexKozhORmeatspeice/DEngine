#include "DEngine.h"

#include "imgui/imgui.h"

class ExampleLayer : public DEngine::Layer
{
public:
    ExampleLayer() 
        : DEngine::Layer("Example"), m_CamPos(0.0f), m_SquarePos(0.0f), m_CamRot(0.0f)
    {
        ///Set camera
		DEngine::Window& win = DEngine::Application::Get().GetWindow();
        float camScaleModif = 0.1f;

        m_Camera = std::make_shared<DEngine::PerspectiveCamera>(60.0f, win.GetWidth() * camScaleModif, win.GetHeight() * camScaleModif);

        //Set Renderer
        ///Init buffers
        m_VertexArray.reset(DEngine::VertexArray::Create());

        float verts[8 * 9] = {
			// Передняя грань (z = 0.5f) - UV: 0,0 до 1,1
			-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f,  // 0
			 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 1.0f,  1.0f, 0.0f,  // 1
			 0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f,  // 2
			-0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f, 1.0f,  0.0f, 1.0f,  // 3
			
			// Задняя грань (z = -0.5f) - UV: 0,0 до 1,1
			-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f, 1.0f,  0.0f, 0.0f,  // 4
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f, 1.0f,  1.0f, 0.0f,  // 5
			 0.5f,  0.5f, -0.5f,  1.0f, 0.5f, 0.0f, 1.0f,  1.0f, 1.0f,  // 6
			-0.5f,  0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f,  0.0f, 1.0f   // 7
		};


        m_VertexBuffer.reset(DEngine::VertexBuffer::Create(verts, sizeof(verts)));
        DEngine::BufferLayout layout =
        {
            {DEngine::ShaderDataType::Float3, "a_Position"},
            {DEngine::ShaderDataType::Float4, "a_Color"},
            {DEngine::ShaderDataType::Float2, "a_TexCoord"},
        };
        m_VertexBuffer->SetLayout(layout);
        m_VertexArray->AddVertexBuffer(m_VertexBuffer);

        unsigned int inds[36] = {
			0, 1, 2,
			2, 3, 0,
			4, 6, 5,
			4, 7, 6,
			3, 7, 0,
			7, 4, 0,
			1, 5, 6,
			1, 6, 2,
			3, 2, 6,
			3, 6, 7,
			0, 5, 1,
			0, 4, 5
		};

        m_IndexBuffer.reset(DEngine::IndexBuffer::Create(inds, sizeof(inds) / sizeof(uint32_t)));
        m_VertexArray->SetIndexBuffer(m_IndexBuffer);

        ///Init shaders
        std::string vertSrc = R"(
            #version 330 core

            layout(location = 0) in vec3 a_Position;
            layout(location = 1) in vec4 a_Color;
            layout(location = 1) in vec2 a_Texcoord;

            uniform mat4 u_ViewProj;
            uniform mat4 u_ModelMat;

            out vec3 v_Pos;
            out vec4 v_Color;
            out vec2 v_Texcoord;
            
            void main()
			{
                v_Pos = a_Position;
                v_Color = a_Color;
                v_Texcoord = a_Texcoord;
                gl_Position = u_ViewProj * u_ModelMat * vec4(a_Position, 1.0f);
			}
        )";

        std::string fragSrc = R"(
            #version 330 core

            layout(location = 0) out vec4 color;

            uniform sampler2D u_Texture;

            in vec3 v_Pos;
            in vec4 v_Color;
            in vec2 v_Texcoord;
            
            void main()
			{
                color = texture(u_Texture, v_Texcoord);
			}
        )";

        m_Shader.reset(DEngine::Shader::Create(vertSrc, fragSrc));

		m_Texture = DEngine::Texture2D::Create("assets/textures/Shoot.png");

        m_Shader->UploadUniformInt("u_Texture", 0);
    }

	virtual void OnUpdate(const DEngine::Timestep& ts) override 
    {
        if (DEngine::Input::IsKeyPressed(D_KEY_LEFT))
            m_CamPos = m_CamPos - m_Camera->GetRightDir() * m_CamSpeed * ts.GetSeconds();
        if (DEngine::Input::IsKeyPressed(D_KEY_RIGHT))
            m_CamPos = m_CamPos + m_Camera->GetRightDir() * m_CamSpeed * ts.GetSeconds();
        if (DEngine::Input::IsKeyPressed(D_KEY_UP))
            m_CamPos = m_CamPos + m_Camera->GetForwardDir() * m_CamSpeed * ts.GetSeconds();
        if (DEngine::Input::IsKeyPressed(D_KEY_DOWN))
            m_CamPos = m_CamPos - m_Camera->GetForwardDir() * m_CamSpeed * ts.GetSeconds();

		//D_INFO("Got cam pos {0}, {1}, {2}", m_CamPos.x, m_CamPos.y, m_CamPos.z);

        if (DEngine::Input::IsKeyPressed(D_KEY_A))
            m_CamRot.y += m_CamRotSpeed * ts;
        if (DEngine::Input::IsKeyPressed(D_KEY_D))
            m_CamRot.y -= m_CamRotSpeed * ts;
        if (DEngine::Input::IsKeyPressed(D_KEY_W))
            m_CamRot.x += m_CamRotSpeed * ts;
        if (DEngine::Input::IsKeyPressed(D_KEY_S))
            m_CamRot.x -= m_CamRotSpeed * ts;

        m_Camera->SetPos(m_CamPos);
        m_Camera->SetRot(m_CamRot);
        
        glm::mat4 trans = glm::translate(glm::mat4(1.0f), m_CamPos);

		DEngine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.15f, 1.0f });
		DEngine::RenderCommand::Clear();

		DEngine::Renderer::BeginScene(m_Camera);

        m_Texture->Bind();
		DEngine::Renderer::Submit(m_VertexArray, m_Shader);

		DEngine::Renderer::EndScene();
    }

	virtual void OnImGuiRenderer() override
    {
        ImGui::Begin("HUI");
        ImGui::End();
    }

	virtual void OnEvent(DEngine::Event& event) override
    {
        DEngine::EventDispatcher dis(event);
        dis.Dispatch<DEngine::KeyPressedEvent>(BIND_EVENT_FN(ExampleLayer::OnKeyPressedEv));
    }

    bool OnKeyPressedEv(DEngine::KeyPressedEvent& event)
    {
        return false;
    }
private:
    DEngine::Ref<DEngine::Camera> m_Camera;

	DEngine::Ref<DEngine::VertexArray> m_VertexArray;
	DEngine::Ref<DEngine::VertexBuffer> m_VertexBuffer;
	DEngine::Ref<DEngine::IndexBuffer> m_IndexBuffer;

	DEngine::Ref<DEngine::Shader> m_Shader;
    DEngine::Ref<DEngine::Texture2D> m_Texture;

    glm::vec3 m_CamPos;
    glm::vec3 m_CamRot;
    float m_CamSpeed = 2.0f;
    float m_CamRotSpeed = 100.0f;

    glm::vec3 m_SquarePos;
};

class SandBox : public DEngine::Application
{
public:
    SandBox() : DEngine::Application() 
    {
        PushLayer(new ExampleLayer());
    }

    virtual ~SandBox() {}
};

DEngine::Application* DEngine::CreateApplication()
{
	return new SandBox();
}
