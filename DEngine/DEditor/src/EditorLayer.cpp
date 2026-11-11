#include "EditorLayer.h"

#include "Scene/SceneGenerator.h"
#include "DEngine/Profiler/Profiler.h"

namespace DEngine
{
	EditorLayer::EditorLayer()
		: Layer("Editor")
	{
	}

	void EditorLayer::Init()
	{
		AssetManager::Init(std::make_shared<EditorAssetManager>());

		const Window& win = Application::Get().GetWindow();
		m_Camera.Init(win.GetWidth(), win.GetHeight());
		m_Camera.SetViewport(&m_EditorViewport);
		m_EditorViewport.Init(m_Camera.GetCamera());
		m_GameViewport.Init();

		const AssetHandle sceneHandle = AssetManager::CreateAsset(SceneGenerator::BaseScenePath());
		m_ActiveScene = AssetManager::GetAsset<Scene>(sceneHandle);
		if (!m_ActiveScene)
			m_ActiveScene = SceneGenerator::Create();

		m_ScenePanel.SetContext(m_ActiveScene);
		m_PropPanel.SetContext(m_ActiveScene);
		m_AssetsPanel.SetContext(m_ActiveScene);
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

	void EditorLayer::DeserializeScene()
	{
		m_CurrentGameMode = EDITOR;
		m_PlaySnapshot.clear();
		AssetManager::CreateAsset(SceneGenerator::BaseScenePath());
		DeserializeSceneResult loaded = SceneSerializer::Deserialize(SceneGenerator::BaseScenePath());
		if (!loaded.isSuccessful || !loaded.scene)
		{
			D_ERROR("Couldn't deserialize the scene");
			return;
		}

		BindScene(loaded.scene, true);
	}

	void EditorLayer::EnablePhysics()
	{
		auto comps = m_ActiveScene->View<TagComponent>();
		for (auto [entity, tagcomp] : comps.each())
		{
			(void)entity;
			tagcomp.isEnabled = true;
		}
	}

	void EditorLayer::DisablePhysics()
	{
		auto comps = m_ActiveScene->View<TagComponent>();
		for (auto [entity, tagcomp] : comps.each())
		{
			(void)entity;
			tagcomp.isEnabled = false;
		}
	}

	void EditorLayer::DisableSelectedEntityDebug()
	{
		Entity selection = m_PropPanel.getSelectedEntity();
		if (selection && selection.HasComponent<DebugComponent>())
			selection.GetComponent<DebugComponent>().debugOn = false;
	}

	void EditorLayer::EnableSelectedEntityDebug()
	{
		Entity selection = m_PropPanel.getSelectedEntity();
		if (selection && selection.HasComponent<DebugComponent>())
			selection.GetComponent<DebugComponent>().debugOn = true;
	}

	void EditorLayer::OnUpdate(const Timestep& ts)
	{
		D_PROFILE_FUNCTION();
		UpdateAssets(ts);
		m_DrawTime = ts.GetSeconds();

		if (m_ActiveScene)
		{
			if (auto cameraSystem = m_ActiveScene->GetSystem<CameraSystem>())
				cameraSystem->SetViewport((uint32_t)m_GameViewport.GetSize().x, (uint32_t)m_GameViewport.GetSize().y);
			m_ActiveScene->OnUpdate(ts);
		}

		m_Camera.OnUpdate(ts);
		m_EditorViewport.OnUpdate(ts);
	}

	void EditorLayer::OnRender(const Timestep& ts)
	{
		D_PROFILE_FUNCTION();
		if (!m_ActiveScene)
			return;

		Ref<Camera> gameCamera = m_Camera.GetCamera();
		bool usesSceneCamera = false;
		if (auto cameraSystem = m_ActiveScene->GetSystem<CameraSystem>())
		{
			if (cameraSystem->HasSceneCamera())
			{
				gameCamera = cameraSystem->GetSceneCamera();
				usesSceneCamera = true;
			}
		}

		m_GameViewport.SetUsesSceneCamera(usesSceneCamera);
		m_EditorViewport.RenderGraphic(ts, m_ActiveScene);
		m_GameViewport.RenderGraphic(ts, m_ActiveScene, gameCamera);
	}

	void EditorLayer::OnImGuiRenderer()
	{
		D_PROFILE_FUNCTION();

		EditorMenuActions actions;
		actions.Play = [this]() { StartGame(); };
		actions.Stop = [this]() { StopGame(); };
		actions.Serialize = [this]() { SceneSerializer::Serialize(m_ActiveScene, SceneGenerator::BaseScenePath()); };
		actions.Deserialize = [this]() { DeserializeScene(); };
		actions.Exit = []() { Application::Get().Close(); };

		m_Dockspace.OnRender([&]()
		{
			m_Menu.OnRender(m_CurrentGameMode, actions);
			m_ScenePanel.OnRender();
			m_PropPanel.OnRender();
			m_AssetsPanel.OnRender();
			m_SceneInfo.OnRender(m_ActiveScene);
			m_GameViewport.OnRender(m_ActiveScene);
			m_EditorViewport.OnRender(m_ActiveScene, m_PropPanel);
			m_Profile.OnRender(m_DrawTime);
		});
	}

	void EditorLayer::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(EditorLayer::OnMouseButtonPressedEvent));
		dispatcher.Dispatch<MouseButtonReleasedEvent>(BIND_EVENT_FN(EditorLayer::OnMouseButtonReleasedEvent));
		dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(EditorLayer::OnMouseMovedEvent));
		dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(EditorLayer::OnMouseScrolledEvent));
	}

	bool EditorLayer::OnMouseButtonPressedEvent(MouseButtonPressedEvent& event)
	{
		return m_Camera.OnMousePressed(event);
	}

	bool EditorLayer::OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& event)
	{
		return m_Camera.OnMouseReleased(event);
	}

	bool EditorLayer::OnMouseMovedEvent(MouseMovedEvent& event)
	{
		return m_Camera.OnMouseMoved(event);
	}

	bool EditorLayer::OnMouseScrolledEvent(MouseScrolledEvent& event)
	{
		return m_Camera.OnMouseScrolled(event);
	}

	void EditorLayer::Shutdown()
	{
		AssetManager::Shutdown();
	}

	void EditorLayer::UpdateAssets(const Timestep& ts)
	{
		m_TimeSinceAssetUpdate += ts.GetSeconds();
		if (m_TimeSinceAssetUpdate >= m_AssetReloadInterval)
		{
			AssetManager::Update();
			m_TimeSinceAssetUpdate = 0.0f;
		}
	}
}
