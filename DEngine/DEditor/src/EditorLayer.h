#pragma once
#include "DEngine.h"
#include "Panels/Scene/ScenePanel.h"
#include "Panels/Properties/PropertiesPanel.h"
#include "Panels/AssetsPanel/AssetsPanel.h"

namespace DEngine
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();


		virtual void Init() override;
		virtual void OnUpdate(const Timestep& ts) override;
		virtual void OnRender(const Timestep& ts) override;
		void OnRenderDocker();
		virtual void Shutdown() override;

		virtual void OnEvent(Event& event) override;
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
		Ref<Camera> m_EditorCamera;
		Ref<Framebuffer> m_Framebuffer;
		Ref<Scene> m_ActiveScene;

		glm::vec2 m_ViewportSize;
		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;

		glm::vec3 m_CamPos;
		glm::vec3 m_CamRot;
		float m_CamSpeed = 200.0f;
		float m_CamRotSpeed = 100.0f;

		float timeUpdateAssetsSum = 0.0f;
		const float TIME_BETWEEN_ASSETS_HOT_RELOAD = 1.0f;

		glm::vec3 m_SquarePos;
	};
}
