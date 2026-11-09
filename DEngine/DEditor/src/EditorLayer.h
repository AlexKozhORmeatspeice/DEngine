#pragma once
#include "DEngine.h"
#include "Panels/Scene/ScenePanel.h"
#include "Panels/Properties/PropertiesPanel.h"
#include "Panels/AssetsPanel/AssetsPanel.h"

#include "ImGuizmo.h"

namespace DEngine
{
	enum GameMode {
		EDITOR,
		GAME
	};

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();


		virtual void Init() override;
		virtual void OnUpdate(const Timestep& ts) override;
		virtual void OnRender(const Timestep& ts) override;
		void OnRenderDocker();
		virtual void Shutdown() override;

		void RenderToTarget(const Ref<Framebuffer>& framebuffer, const Ref<Camera>& camera, const Timestep& ts);
		void RenderImGuizmo();

		void LoadScene();
		void StartGame();
		void StopGame();
		void BindScene(const Ref<Scene>& scene, bool selectFirst);
		void EnablePhysics();
		void DisablePhysics();
		void DisableSelectedEntityDebug();
		void EnableSelectedEntityDebug();

		void UpdateEditorCamera(const Timestep& ts);
		void DeserializeScene();
		void AcceptModelDropOnLastItem();

		virtual void OnEvent(Event& event) override;

		bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& event);
		bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& event);
		bool OnMouseMovedEvent(MouseMovedEvent& event);
		bool OnMouseScrolledEvent(MouseScrolledEvent& event);

		bool OnKeyPressedEv(KeyPressedEvent& event);

		virtual void OnImGuiRenderer() override;
	private:
		void UpdateAssets(const Timestep& ts);
	private:
		//Panels
		ScenePanel m_ScenePanel;
		PropetiesPanel m_PropPanel;
		AssetsPanel m_AssetsPanel;

		//Scene management
		SceneSerializer m_SceneSerializer;

		//Objs
		Ref<PerspectiveCamera> m_EditorCamera;

		Ref<Framebuffer> m_EditorFramebuffer;
		Ref<Framebuffer> m_GameFramebuffer;
		Ref<Scene> m_ActiveScene;

		glm::vec2 m_EditorViewportSize = { 0.0f, 0.0f };
		glm::vec2 m_GameViewportSize = { 0.0f, 0.0f };
		bool m_EditorViewportFocused = false;
		bool m_EditorViewportHovered = false;
		bool m_GameViewUsesSceneCamera = false;

		glm::vec3 m_CamPos;
		glm::vec3 m_CamRot;
		float m_CamSpeed = 200.0f;

		float timeUpdateAssetsSum = 0.0f;
		const float TIME_BETWEEN_ASSETS_HOT_RELOAD = 1.0f;

		glm::vec3 m_SquarePos;

		GameMode m_CurrentGameMode = GameMode::EDITOR;
		bool m_ControllingCameraWithMouse = false;
		bool m_TabJustPressed = false;
		std::string m_PlaySnapshot;
		float m_DrawTime;

		float m_LastMouseX = 0.0f;
		float m_LastMouseY = 0.0f;
		bool m_FirstMouseMove = true;

		float m_MouseSensitivity = 0.15f;

		ImGuizmo::OPERATION m_GuizmoType = ImGuizmo::OPERATION::TRANSLATE;

	};
}