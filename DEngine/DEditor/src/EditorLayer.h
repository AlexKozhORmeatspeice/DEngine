#pragma once
#include "DEngine.h"

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
		virtual void Shutdown() override;

		virtual void OnEvent(Event& event) override;

		bool OnKeyPressedEv(KeyPressedEvent& event);
	private:
		Ref<Camera> m_EditorCamera;

		Ref<Framebuffer> m_Framebuffer;

		Ref<Scene> m_Scene;

		glm::vec2 m_ViewportSize;
		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;

		glm::vec3 m_CamPos;
		glm::vec3 m_CamRot;
		float m_CamSpeed = 2.0f;
		float m_CamRotSpeed = 100.0f;

		glm::vec3 m_SquarePos;
	};
}
