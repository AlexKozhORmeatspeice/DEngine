#include "EditorLayer.h"
#include "Panels/AssetDragDrop.h"
#include "imgui/imgui.h"
#include "DEngine/Core.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "DEngine/Input/MouseButtonCodes.h"

#define BASE_SCENE_PATH "assets/scenes/Example.dscene"

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
		AssetHandle m_SceneHandle = AssetManager::CreateAsset(BASE_SCENE_PATH);
		m_ActiveScene = AssetManager::GetAsset<Scene>(m_SceneHandle);

		//Создание сцены, если она еще не была создана
		if (!m_ActiveScene)
		{
			LoadScene();
		}

		//Set Renderer
		m_EditorFramebuffer = Framebuffer::Create({ win.GetWidth(), win.GetHeight() });
		m_GameFramebuffer = Framebuffer::Create({ win.GetWidth(), win.GetHeight() });


		//Set panels
		m_ScenePanel.SetContext(m_ActiveScene);
		m_PropPanel.SetContext(m_ActiveScene);
		m_AssetsPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::LoadScene() 
	{
		m_ActiveScene = CreateRef<Scene>();

		///Set models
		const AssetHandle texHandle = AssetManager::CreateAsset("assets/textures/pasha.jpg");

		Ref<Material> mat = CreateRef<Material>(AssetManager::GetBaseRendererShaderHandle());
		mat->SetTexture2D("u_Texture", texHandle);

		Ref<Material> mat2 = CreateRef<Material>(AssetManager::GetBaseRendererShaderHandle());

		std::string matName = "basemat";
		std::string matName2 = "basemat2";
		matName += AssetHelper::NativeExtension(AssetType::Material);
		matName2 += AssetHelper::NativeExtension(AssetType::Material);
		const AssetHandle matHandle = AssetManager::CreateMaterialAsset(mat, "assets/materials/" + matName);
		const AssetHandle matHandle2 = AssetManager::CreateMaterialAsset(mat2, "assets/materials/" + matName2);

		const AssetHandle sponzaHandle = AssetManager::CreateAsset({ AssetType::Model, "assets/models/sponza.obj-master/sponza.obj" });
		const AssetHandle meshHandle = AssetManager::GetPrimitiveMesh(PrimitiveType::Cube);

		const AssetHandle sphereMeshHandle = AssetManager::GetPrimitiveMesh(PrimitiveType::Sphere);

		///Set objs
		auto& cube = m_ActiveScene->CreateEntity("cubee");
		cube.AddComponent<MeshRendererComponent>(meshHandle, matHandle);
		cube.AddComponent<RigidbodyComponent>().isKinematic = true;
		cube.AddComponent<ColliderComponent>();

		auto& trans = cube.GetComponent<TransformComponent>();
		trans.SetScale({ 100.0f, 100.0f, 100.0f });
		trans.SetPosition({ 0.0f, 0.0f, -200.0f });
		

		auto& cube2 = m_ActiveScene->CreateEntity("cube2");
		cube2.AddComponent<MeshRendererComponent>(meshHandle, matHandle2);
		cube2.AddComponent<RigidbodyComponent>().isKinematic = true;
		cube2.AddComponent<ColliderComponent>();

		auto& trans2 = cube2.GetComponent<TransformComponent>();
		trans2.SetScale({ 400.0f, 50.0f, 400.0f });
		trans2.SetPosition({ 0.0f, -100.0f, 0.0f });


		auto& cube3 = m_ActiveScene->CreateEntity("cube3");
		cube3.AddComponent<MeshRendererComponent>(meshHandle, matHandle);
		cube3.AddComponent<RigidbodyComponent>().isKinematic = true;
		cube3.AddComponent<ColliderComponent>();

		auto& trans3 = cube3.GetComponent<TransformComponent>();
		trans3.SetScale({ 100.0f, 100.0f, 100.0f });
		trans3.SetPosition({ 0.0f, 500.0f, -200.0f });


		auto& sphere1 = m_ActiveScene->CreateEntity("sphere1");
		sphere1.AddComponent<MeshRendererComponent>(sphereMeshHandle, matHandle);
		sphere1.AddComponent<RigidbodyComponent>().isKinematic = true;
		sphere1.AddComponent<ColliderComponent>();

		auto& sphere1trans = sphere1.GetComponent<TransformComponent>();
		sphere1trans.SetScale({ 100.0f, 100.0f, 100.0f });
		sphere1trans.SetPosition({ 50.0f, 200.0f, -100.0f });

		auto& sphere2 = m_ActiveScene->CreateEntity("sphere2");
		sphere2.AddComponent<MeshRendererComponent>(sphereMeshHandle, matHandle);
		sphere2.AddComponent<RigidbodyComponent>().isKinematic = true;
		sphere2.AddComponent<ColliderComponent>();

		auto& sphere2trans = sphere2.GetComponent<TransformComponent>();
		sphere2trans.SetScale({ 100.0f, 100.0f, 100.0f });
		sphere2trans.SetPosition({ 50.0f, 400.0f, -100.0f });

		auto& directLight = m_ActiveScene->CreateEntity("direct light");
		directLight.AddComponent<DirectLightComponent>(glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
		auto& lightTrans = directLight.GetComponent<TransformComponent>();
		lightTrans.Rotate(70.0f, { 0.0f, 1.0f, 0.0f });

		auto& mainCamera = m_ActiveScene->CreateEntity("Main Camera");
		mainCamera.AddComponent<CameraComponent>();
		auto& cameraTrans = mainCamera.GetComponent<TransformComponent>();
		cameraTrans.SetPosition({ 0.0f, 150.0f, 250.0f });

		AssetManager::CreateSceneAsset(m_ActiveScene, BASE_SCENE_PATH);
	}

	void EditorLayer::StartGame()
	{
		if (m_CurrentGameMode == GAME || !m_ActiveScene)
			return;

		m_PlaySnapshot = SceneSerializer::SerializeToString(m_ActiveScene);
		if (m_PlaySnapshot.empty())
		{
			D_ERROR("Couldn't start play mode");
			return;
		}

		m_CurrentGameMode = GAME;
		DisableSelectedEntityDebug();
		EnablePhysics();
		D_INFO("SWITCHED TO PLAY MODE");
	}

	void EditorLayer::StopGame()
	{
		if (m_CurrentGameMode != GAME)
			return;

		DeserializeSceneResult restored = SceneSerializer::DeserializeFromString(m_PlaySnapshot);
		m_CurrentGameMode = EDITOR;
		m_PlaySnapshot.clear();

		if (!restored.isSuccessful || !restored.scene)
		{
			D_ERROR("Couldn't restore the editor scene");
			DisablePhysics();
			return;
		}

		BindScene(restored.scene, true);
		DisablePhysics();
		EnableSelectedEntityDebug();
		D_INFO("SWITCHED TO EDITOR MODE");
	}

	void EditorLayer::BindScene(const Ref<Scene>& scene, bool selectFirst)
	{
		m_ActiveScene = scene;
		m_ScenePanel.SetContext(scene);
		m_PropPanel.SetContext(scene);
		m_AssetsPanel.SetContext(scene);

		if (selectFirst && scene && !scene->GetAllEntities().empty())
			m_ScenePanel.SetSelected(scene->GetAllEntities().front());
		else
			m_ScenePanel.SetSelected({});
	}

	void EditorLayer::EnablePhysics()
	{
		auto comps = m_ActiveScene->View<TagComponent>();

		for (auto [entity, tagcomp] : comps.each())
		{
			tagcomp.isEnabled = true;
		}
	}

	void EditorLayer::DisablePhysics()
	{
		auto comps = m_ActiveScene->View<TagComponent>();

		for (auto [entity, tagcomp] : comps.each())
		{
			tagcomp.isEnabled = false;
		}
	}

	void EditorLayer::DisableSelectedEntityDebug()
	{
		Entity selection = m_PropPanel.getSelectedEntity();
		if (selection && selection.HasComponent<DebugComponent>())
		{
			auto& debug = selection.GetComponent<DebugComponent>();
			debug.debugOn = false;
		}
	}

	void EditorLayer::EnableSelectedEntityDebug()
	{
		Entity selection = m_PropPanel.getSelectedEntity();
		if (selection && selection.HasComponent<DebugComponent>())
		{
			auto& debug = selection.GetComponent<DebugComponent>();
			debug.debugOn = true;
		}
	}

	void EditorLayer::OnUpdate(const Timestep& ts)
	{
		UpdateAssets(ts);

		m_DrawTime = ts.GetSeconds();
		if (auto cameraSystem = m_ActiveScene->GetSystem<CameraSystem>())
			cameraSystem->SetViewport((uint32_t)m_GameViewportSize.x, (uint32_t)m_GameViewportSize.y);
		m_ActiveScene->OnUpdate(ts);
		UpdateEditorCamera(ts);


		// ОБРАБОТКА ОСТАЛЬНОГО ИНПУТА
		if (Input::IsKeyPressed(D_KEY_TAB)) {
			if (!m_TabJustPressed)
			{
				m_TabJustPressed = true;
				m_GuizmoType = m_GuizmoType == ImGuizmo::OPERATION::TRANSLATE ? ImGuizmo::OPERATION::ROTATE : m_GuizmoType == ImGuizmo::OPERATION::ROTATE ? ImGuizmo::OPERATION::SCALE : ImGuizmo::OPERATION::TRANSLATE;
			}
		}
		else
		{
			m_TabJustPressed = false;
		}

	}

	void EditorLayer::UpdateEditorCamera(const Timestep& ts)
	{
		if (m_EditorViewportFocused)
		{
			glm::vec3 forward = m_EditorCamera->GetForwardDir();
			glm::vec3 right = m_EditorCamera->GetRightDir();
			float step = m_CamSpeed * ts.GetSeconds();

			if (Input::IsKeyPressed(D_KEY_W))
				m_CamPos += forward * step;
			if (Input::IsKeyPressed(D_KEY_S))
				m_CamPos -= forward * step;
			if (Input::IsKeyPressed(D_KEY_A))
				m_CamPos -= right * step;
			if (Input::IsKeyPressed(D_KEY_D))
				m_CamPos += right * step;

			m_EditorCamera->SetPos(m_CamPos);
			m_EditorCamera->SetRot(m_CamRot);
		}
	}

	void EditorLayer::DeserializeScene()
	{
		m_CurrentGameMode = EDITOR;
		m_PlaySnapshot.clear();
		AssetManager::CreateAsset(BASE_SCENE_PATH);
		DeserializeSceneResult loaded = SceneSerializer::Deserialize(BASE_SCENE_PATH);
		if (!loaded.isSuccessful || !loaded.scene)
		{
			D_ERROR("Couldn't deserialize the scene");
			return;
		}

		BindScene(loaded.scene, true);
	}

	void EditorLayer::RenderToTarget(const Ref<Framebuffer>& framebuffer, const Ref<Camera>& camera, const Timestep& ts)
	{
		if (!framebuffer || !camera)
			return;

		framebuffer->Bind();
		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.15f, 1.0f });
		RenderCommand::Clear();
		Renderer::BeginScene(camera);
		m_ActiveScene->OnRender(ts);
		Renderer::EndScene();
		framebuffer->Unbind();
	}

	void EditorLayer::OnRender(const Timestep& ts)
	{
		Ref<Camera> gameCamera = m_EditorCamera;
		m_GameViewUsesSceneCamera = false;
		if (auto cameraSystem = m_ActiveScene->GetSystem<CameraSystem>())
		{
			if (cameraSystem->HasSceneCamera())
			{
				gameCamera = cameraSystem->GetSceneCamera();
				m_GameViewUsesSceneCamera = true;
			}
		}

		RenderToTarget(m_EditorFramebuffer, m_EditorCamera, ts);
		RenderToTarget(m_GameFramebuffer, gameCamera, ts);
	}

	void EditorLayer::OnRenderDocker()
	{
		//Обработка панелей
		m_ScenePanel.OnImGuiRender();
		m_PropPanel.OnImGuiRender();
		m_AssetsPanel.OnImGuiRender();

		// Begin ImGuizmo
		ImGuizmo::BeginFrame();

		//Менюшка
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Serialize"))
				{
					SceneSerializer::Serialize(m_ActiveScene, BASE_SCENE_PATH);
				}

				if (ImGui::MenuItem("Deserialize"))
				{
					DeserializeScene();
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		//Профайлинг
		ImGui::Begin("Profile data");
		char fpsLabel[50];
		char spfLabel[50];
		int fps = (int)Application::Get().GetFPS();
		strcpy(fpsLabel, std::to_string(fps).c_str());
		strcpy(spfLabel, std::to_string(m_DrawTime).c_str());
		strcat(fpsLabel, " FPS");
		strcat(spfLabel, " SPF");
		ImGui::Text(fpsLabel);
		ImGui::Text(spfLabel);
		ImGui::End();

		ImGui::Begin("Scene info");
		int objCount = m_ActiveScene->GetAllEntities().size();
		char objCountLabel[50];
		strcpy(objCountLabel, std::to_string(objCount).c_str());
		strcat(objCountLabel, " Objects on scene");
		ImGui::Text(objCountLabel);
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
		ImGui::Begin("Editor");

		m_EditorViewportFocused = ImGui::IsWindowFocused();
		m_EditorViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->SetBlockEvents(!m_EditorViewportFocused || !m_EditorViewportHovered);

		ImVec2 editorSize = ImGui::GetContentRegionAvail();
		if (editorSize.x > 0.0f && editorSize.y > 0.0f && m_EditorViewportSize != *((glm::vec2*)&editorSize))
		{
			m_EditorViewportSize = { editorSize.x, editorSize.y };
			m_EditorFramebuffer->Resize((uint32_t)editorSize.x, (uint32_t)editorSize.y);
			m_EditorCamera->ChangeSize((uint32_t)editorSize.x, (uint32_t)editorSize.y);
		}

		ImGui::Image((void*)m_EditorFramebuffer->GetColorAttachmentRendererID(), ImVec2{ editorSize.x, editorSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		AcceptModelDropOnLastItem();
		RenderImGuizmo();

		ImGui::End();

		ImGui::Begin("Game");
		ImVec2 gameSize = ImGui::GetContentRegionAvail();
		if (gameSize.x > 0.0f && gameSize.y > 0.0f && m_GameViewportSize != *((glm::vec2*)&gameSize))
		{
			m_GameViewportSize = { gameSize.x, gameSize.y };
			m_GameFramebuffer->Resize((uint32_t)gameSize.x, (uint32_t)gameSize.y);
		}

		ImGui::Image((void*)m_GameFramebuffer->GetColorAttachmentRendererID(), ImVec2{ gameSize.x, gameSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		AcceptModelDropOnLastItem();
		if (!m_GameViewUsesSceneCamera)
		{
			ImGui::SetCursorPos(ImVec2(8.0f, 8.0f));
			ImGui::TextDisabled("No scene camera");
		}
		ImGui::End();
		ImGui::PopStyleVar();
	}

	void EditorLayer::RenderImGuizmo()
	{
		if (m_PropPanel.getSelectedEntity())
		{
			Entity selected_entity = m_PropPanel.getSelectedEntity();
			if (selected_entity)
			{
				ImVec2 winpos = ImGui::GetWindowPos();
				float windowWidth = (float)ImGui::GetWindowWidth();
				float windowHeight = (float)ImGui::GetWindowHeight();

				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetDrawlist();
				ImGuizmo::SetRect(winpos.x, winpos.y, windowWidth, windowHeight);

				glm::mat4 camViewMat = m_EditorCamera->GetViewMat();
				glm::mat4 camProjMat = m_EditorCamera->GetProjMat();

				auto& entityTransform = selected_entity.GetComponent<TransformComponent>().GetModelMatrixLink();

				ImGuizmo::Manipulate(glm::value_ptr(camViewMat), glm::value_ptr(camProjMat),
					(ImGuizmo::OPERATION)m_GuizmoType, ImGuizmo::WORLD, glm::value_ptr(entityTransform));
			}
		}
	}

	void EditorLayer::OnEvent(Event& event)
	{
		EventDispatcher dis(event);
		dis.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::OnKeyPressedEv));
		dis.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(EditorLayer::OnMouseButtonPressedEvent));
		dis.Dispatch<MouseButtonReleasedEvent>(BIND_EVENT_FN(EditorLayer::OnMouseButtonReleasedEvent));
		dis.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(EditorLayer::OnMouseMovedEvent));
		dis.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(EditorLayer::OnMouseScrolledEvent));

	}

	bool EditorLayer::OnMouseButtonPressedEvent(MouseButtonPressedEvent& event)
	{
		if (event.GetMouseButton() == D_MOUSE_BUTTON_RIGHT)
		{
			m_ControllingCameraWithMouse = true;
			m_FirstMouseMove = true;
			return true;
		}

		return false;
	}

	bool EditorLayer::OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& event)
	{
		if (event.GetMouseButton() == D_MOUSE_BUTTON_RIGHT)
		{
			m_ControllingCameraWithMouse = false;
			m_FirstMouseMove = true;
			return true;
		}

		return false;
	}

	bool EditorLayer::OnMouseMovedEvent(MouseMovedEvent& event)
	{
		if (!m_EditorViewportFocused || !m_EditorViewportHovered) return false;
		if (!m_ControllingCameraWithMouse) return false;

		float x = event.GetX();
		float y = event.GetY();

		if (m_FirstMouseMove)
		{
			m_LastMouseX = x;
			m_LastMouseY = y;
			m_FirstMouseMove = false;
			return false;
		}

		float deltaX = x - m_LastMouseX;
		float deltaY = y - m_LastMouseY;

		m_LastMouseX = x;
		m_LastMouseY = y;

		m_CamRot.y -= deltaX * m_MouseSensitivity;
		m_CamRot.x -= deltaY * m_MouseSensitivity;

		// Ограничиваем pitch, чтобы камера не переворачивалась
		m_CamRot.x = glm::clamp(m_CamRot.x, -89.0f, 89.0f);

		m_EditorCamera->SetRot(m_CamRot);

		return true;
	}

	bool EditorLayer::OnMouseScrolledEvent(MouseScrolledEvent& event)
	{
		if (!m_EditorViewportFocused || !m_EditorViewportHovered) return false;

		float offset = event.GetYOffset();
		m_CamPos += m_EditorCamera->GetForwardDir() * offset * 20.0f;

		m_EditorCamera->SetPos(m_CamPos);

		return true;
	}

	void EditorLayer::AcceptModelDropOnLastItem()
	{
		AcceptModelDrop(m_ActiveScene);
	}

	void EditorLayer::Shutdown()
	{
		AssetManager::Shutdown();
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
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
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
				if (ImGui::MenuItem("Exit")) Application::Get().Close();
				ImGui::EndMenu();
			}

			ImGui::SameLine();
			if (m_CurrentGameMode == GameMode::EDITOR)
			{
				if (ImGui::Button("Play"))
					StartGame();
			}
			else if (ImGui::Button("Stop"))
			{
				StopGame();
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