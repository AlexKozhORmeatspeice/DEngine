#include "DEngine.h"
#include "DEngine/EntryPoint.h"

namespace DEngine
{
	namespace
	{
		constexpr const char* BASE_SCENE_PATH = "assets/scenes/Example.dscene";
	}

	class SandboxLayer : public Layer
	{
	public:
		SandboxLayer()
			: Layer("Sandbox")
		{
		}

		void Init() override
		{
			AssetManager::Init(std::make_shared<EditorAssetManager>());

			Window& window = Application::Get().GetWindow();
			m_FallbackCamera = std::make_shared<PerspectiveCamera>(60.0f, window.GetWidth(), window.GetHeight());
			m_FallbackCamera->SetPos({ 0.0f, 150.0f, 250.0f });

			const AssetHandle sceneHandle = AssetManager::CreateAsset(BASE_SCENE_PATH);
			m_Scene = AssetManager::GetAsset<Scene>(sceneHandle);
			if (!m_Scene)
			{
				D_CORE_ERROR("Sandbox couldn't load {0}", BASE_SCENE_PATH);
				return;
			}

			auto tags = m_Scene->View<TagComponent>();
			for (auto [entity, tag] : tags.each())
			{
				(void)entity;
				tag.isEnabled = true;
			}

			SyncViewport();
		}

		void OnUpdate(const Timestep& ts) override
		{
			if (!m_Scene)
				return;

			SyncViewport();
			m_Scene->OnUpdate(ts);
		}

		void OnRender(const Timestep& ts) override
		{
			RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.15f, 1.0f });
			RenderCommand::Clear();

			if (!m_Scene)
				return;

			Ref<Camera> camera = m_FallbackCamera;
			if (auto cameraSystem = m_Scene->GetSystem<CameraSystem>())
			{
				if (cameraSystem->HasSceneCamera())
					camera = cameraSystem->GetSceneCamera();
			}

			Renderer::BeginScene(camera);
			m_Scene->OnRender(ts);
			Renderer::EndScene();
		}

		void OnEvent(Event& event) override
		{
			if (event.GetEventType() != EventType::WindowResize)
				return;

			WindowResizeEvent& resize = static_cast<WindowResizeEvent&>(event);
			if (m_FallbackCamera && resize.GetWidth() > 0 && resize.GetHeight() > 0)
				m_FallbackCamera->ChangeSize(resize.GetWidth(), resize.GetHeight());
		}

		void Shutdown() override
		{
			m_Scene = nullptr;
			AssetManager::Shutdown();
		}

	private:
		void SyncViewport()
		{
			Window& window = Application::Get().GetWindow();
			const uint32_t width = window.GetWidth();
			const uint32_t height = window.GetHeight();
			if (width == 0 || height == 0)
				return;

			if (m_FallbackCamera)
				m_FallbackCamera->ChangeSize(width, height);

			if (m_Scene)
			{
				if (auto cameraSystem = m_Scene->GetSystem<CameraSystem>())
					cameraSystem->SetViewport(width, height);
			}
		}

		Ref<Scene> m_Scene;
		Ref<PerspectiveCamera> m_FallbackCamera;
	};

	class Sandbox : public Application
	{
	public:
		Sandbox()
			: Application("DEngine")
		{
			PushLayer(new SandboxLayer());
		}
	};

	Application* CreateApplication()
	{
		return new Sandbox();
	}
}
