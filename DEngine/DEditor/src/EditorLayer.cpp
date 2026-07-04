#include "EditorLayer.h"
#include "imgui/imgui.h"

namespace DEngine
{
	EditorLayer::EditorLayer() 
		: Layer("Editor"), m_CamPos(0.0f), m_SquarePos(0.0f), m_CamRot(0.0f)
	{
		///Set camera
		Window& win = Application::Get().GetWindow();
		m_Camera = std::make_shared<PerspectiveCamera>(60.0f, win.GetWidth(), win.GetHeight());

		//Set Renderer
		///Init buffers
		m_VertexArray = VertexArray::Create();
		m_Framebuffer = Framebuffer::Create({ win.GetWidth(), win.GetHeight() });

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


		m_VertexBuffer = VertexBuffer::Create(verts, sizeof(verts));
		BufferLayout layout =
		{
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float4, "a_Color"},
			{ShaderDataType::Float2, "a_TexCoord"},
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

		m_IndexBuffer = IndexBuffer::Create(inds, sizeof(inds) / sizeof(uint32_t));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		///Init shaders
		shaderLib.Load("assets/shaders/Base.glsl");
		m_Texture = Texture2D::Create("assets/textures/pasha.jpg");
		shaderLib.Get("Base")->UploadUniformInt("u_Texture", 0);
	}

	void EditorLayer::OnUpdate(const Timestep& ts)
	{
		if (Input::IsKeyPressed(D_KEY_LEFT))
			m_CamPos = m_CamPos - m_Camera->GetRightDir() * m_CamSpeed * ts.GetSeconds();
		if (Input::IsKeyPressed(D_KEY_RIGHT))
			m_CamPos = m_CamPos + m_Camera->GetRightDir() * m_CamSpeed * ts.GetSeconds();
		if (Input::IsKeyPressed(D_KEY_UP))
			m_CamPos = m_CamPos + m_Camera->GetForwardDir() * m_CamSpeed * ts.GetSeconds();
		if (Input::IsKeyPressed(D_KEY_DOWN))
			m_CamPos = m_CamPos - m_Camera->GetForwardDir() * m_CamSpeed * ts.GetSeconds();

		if (Input::IsKeyPressed(D_KEY_A))
			m_CamRot.y += m_CamRotSpeed * ts;
		if (Input::IsKeyPressed(D_KEY_D))
			m_CamRot.y -= m_CamRotSpeed * ts;
		if (Input::IsKeyPressed(D_KEY_W))
			m_CamRot.x += m_CamRotSpeed * ts;
		if (Input::IsKeyPressed(D_KEY_S))
			m_CamRot.x -= m_CamRotSpeed * ts;

		m_Camera->SetPos(m_CamPos);
		m_Camera->SetRot(m_CamRot);
		
		glm::mat4 trans = glm::translate(glm::mat4(1.0f), m_CamPos);

		m_Framebuffer->Bind();
		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.15f, 1.0f });
		RenderCommand::Clear();
		Renderer::BeginScene(m_Camera);

		m_Texture->Bind();
		Renderer::Submit(m_VertexArray, shaderLib.Get("Base"));

		Renderer::EndScene();
		m_Framebuffer->Unbind();
	}

	void EditorLayer::OnImGuiRenderer()
	{
		static bool dockspaceOpen = true;
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows,
				// which we can't undo at the moment without finer window depth/z control.
				if (ImGui::MenuItem("Exit")) Application::Get().Close();
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::Begin("Profile data");

		for (const auto& res : m_ProfileResults)
		{
			char label[50];
			strcpy(label, res.Name);
			strcat(label, " %.2fms");

			ImGui::Text(label, res.Time);
		}

		uint32_t textureID = m_Texture->GetRendererID();
		ImGui::Image((void*)m_Framebuffer->GetColorAttachmentRendererID(), ImVec2{ 1280.0f, 720.0f }, ImVec2{ 0, 1 }, ImVec2{1, 0});

		m_ProfileResults.clear();
		ImGui::End();

		ImGui::End();
	}

	void EditorLayer::OnEvent(Event& event)
	{
		EventDispatcher dis(event);
		dis.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::OnKeyPressedEv));

		if (event.GetEventType() == EventType::WindowResize)
		{
			WindowResizeEvent& resizeEvent = (WindowResizeEvent&)event;
			uint32_t width = resizeEvent.GetWidth();
			uint32_t height = resizeEvent.GetHeight();

			m_Camera->ChangeSize(width, height);
		}
	}

	bool EditorLayer::OnKeyPressedEv(KeyPressedEvent& event)
	{
		return false;
	}
}
