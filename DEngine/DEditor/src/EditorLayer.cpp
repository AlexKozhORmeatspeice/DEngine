#include "EditorLayer.h"
#include "imgui/imgui.h"
#include "DEngine/Core.h"

namespace DEngine
{
	EditorLayer::EditorLayer() 
		: Layer("Editor"), m_CamPos(0.0f), m_SquarePos(0.0f), m_CamRot(0.0f)
	{
	}

	void EditorLayer::Init()
	{
		//Init asset manager
		AssetManager::Init(std::make_shared<EditorAssetManager>());

		///Set camera
		Window& win = Application::Get().GetWindow();
		m_EditorCamera = std::make_shared<PerspectiveCamera>(60.0f, win.GetWidth(), win.GetHeight());

		//Set scene
		AssetHandle m_SceneHandle = AssetManager::CreateAsset("assets/scenes/Example.dscene");
		m_ActiveScene = AssetManager::GetAsset<Scene>(m_SceneHandle);
		if (!m_ActiveScene)
		{
			m_ActiveScene = CreateRef<Scene>();
			SceneSerializer::Serialize(m_ActiveScene, "assets/scenes/Example.dscene");
		}

		//Set Renderer
		m_Framebuffer = Framebuffer::Create({ win.GetWidth(), win.GetHeight() });

		//Set panels
		m_ScenePanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnUpdate(const Timestep& ts)
	{
		UpdateAssets(ts);

		m_ActiveScene->OnUpdate(ts);

		//TODO: перенести код управления камерой в отдельный класс
		if (m_ViewportFocused)
		{
			if (Input::IsKeyPressed(D_KEY_LEFT))
				m_CamPos = m_CamPos - m_EditorCamera->GetRightDir() * m_CamSpeed * ts.GetSeconds();
			if (Input::IsKeyPressed(D_KEY_RIGHT))
				m_CamPos = m_CamPos + m_EditorCamera->GetRightDir() * m_CamSpeed * ts.GetSeconds();
			if (Input::IsKeyPressed(D_KEY_UP))
				m_CamPos = m_CamPos + m_EditorCamera->GetForwardDir() * m_CamSpeed * ts.GetSeconds();
			if (Input::IsKeyPressed(D_KEY_DOWN))
				m_CamPos = m_CamPos - m_EditorCamera->GetForwardDir() * m_CamSpeed * ts.GetSeconds();

			if (Input::IsKeyPressed(D_KEY_A))
				m_CamRot.y += m_CamRotSpeed * ts;
			if (Input::IsKeyPressed(D_KEY_D))
				m_CamRot.y -= m_CamRotSpeed * ts;
			if (Input::IsKeyPressed(D_KEY_W))
				m_CamRot.x += m_CamRotSpeed * ts;
			if (Input::IsKeyPressed(D_KEY_S))
				m_CamRot.x -= m_CamRotSpeed * ts;

			m_EditorCamera->SetPos(m_CamPos);
			m_EditorCamera->SetRot(m_CamRot);
		}
	}

	void EditorLayer::OnRender(const Timestep& ts)
	{
		m_Framebuffer->Bind();
		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.15f, 1.0f });
		RenderCommand::Clear();
		Renderer::BeginScene(m_EditorCamera);

		m_ActiveScene->OnRender(ts);

		Renderer::EndScene();
		m_Framebuffer->Unbind();
	}

	void EditorLayer::OnRenderDocker()
	{
		//Обработка панелей
		m_ScenePanel.OnImGuiRender();
		m_PropPanel.OnImGuiRender();
		m_AssetsPanel.OnImGuiRender();

		//Профайлинг
		ImGui::Begin("Profile data");
		char fpsLabel[50];
		strcpy(fpsLabel, std::to_string((int)Application::Get().GetFPS()).c_str());
		strcat(fpsLabel, " FPS");
		ImGui::Text(fpsLabel);
		ImGui::End();

		//Viewport
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
		ImGui::Begin("Viewport");

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportFocused = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->SetBlockEvents(!m_ViewportFocused || !m_ViewportHovered);

		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		if (m_ViewportSize != *((glm::vec2*)&viewportSize))
		{
			m_ViewportSize = { viewportSize.x, viewportSize.y };
			m_EditorCamera->ChangeSize(viewportSize.x, viewportSize.y);
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		ImGui::Image((void*)m_Framebuffer->GetColorAttachmentRendererID(), ImVec2{viewportSize.x, viewportSize.y}, ImVec2{ 0, 1 }, ImVec2{1, 0});

		ImGui::End();
		ImGui::PopStyleVar();
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

			m_EditorCamera->ChangeSize(width, height);
		}
	}

	void EditorLayer::Shutdown()
	{
	}

	bool EditorLayer::OnKeyPressedEv(KeyPressedEvent& event)
	{
		return false;
	}

	//Устанавливаем значения для докера
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
		
		ImGui::Begin("DEngine", &dockspaceOpen, window_flags);
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

		OnRenderDocker();

		ImGui::End();
	}

	void EditorLayer::UpdateAssets(const Timestep& ts)
	{
		timeUpdateAssetsSum += ts.GetSeconds();
		if (timeUpdateAssetsSum >= TIME_BETWEEN_ASSETS_HOT_RELOAD)
		{
			AssetManager::Update();
			timeUpdateAssetsSum = 0.0f;
		}
	}
}
