#pragma once

#include "DEngine.h"
#include "Editor/GameMode.h"
#include "Camera/EditorCameraController.h"
#include "Panels/Scene/ScenePanel.h"
#include "Panels/Properties/PropertiesPanel.h"
#include "Panels/AssetsPanel/AssetsPanel.h"
#include "Panels/Dockspace/DockspacePanel.h"
#include "Panels/Menu/EditorMenuPanel.h"
#include "Panels/Profile/ProfilePanel.h"
#include "Panels/SceneInfo/SceneInfoPanel.h"
#include "Panels/Viewport/EditorViewportPanel.h"
#include "Panels/Viewport/GameViewportPanel.h"

namespace DEngine
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();

		virtual void Init() override;
		virtual void OnUpdate(const Timestep& ts) override;
		virtual void OnRender(const Timestep& ts) override;
		virtual void OnImGuiRenderer() override;
		virtual void OnEvent(Event& event) override;
		virtual void Shutdown() override;

	private:
		void BindScene(const Ref<Scene>& scene, bool selectFirst);
		void StartGame();
		void StopGame();
		void DeserializeScene();
		void EnablePhysics();
		void DisablePhysics();
		void DisableSelectedEntityDebug();
		void EnableSelectedEntityDebug();

		bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& event);
		bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& event);
		bool OnMouseMovedEvent(MouseMovedEvent& event);
		bool OnMouseScrolledEvent(MouseScrolledEvent& event);

		ScenePanel m_ScenePanel;
		PropetiesPanel m_PropPanel;
		AssetsPanel m_AssetsPanel;
		DockspacePanel m_Dockspace;
		EditorMenuPanel m_Menu;
		ProfilePanel m_Profile;
		SceneInfoPanel m_SceneInfo;
		EditorViewportPanel m_EditorViewport;
		GameViewportPanel m_GameViewport;
		EditorCameraController m_Camera;

		Ref<Scene> m_ActiveScene;
		GameMode m_CurrentGameMode = GameMode::EDITOR;
		std::string m_PlaySnapshot;
		float m_DrawTime = 0.0f;
	};
}
