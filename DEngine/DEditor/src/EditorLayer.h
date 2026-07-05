#pragma once
#include "DEngine.h"

namespace DEngine
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();

		virtual void OnUpdate(const Timestep& ts) override;
		virtual void OnImGuiRenderer() override;

		virtual void OnEvent(Event& event) override;

		bool OnKeyPressedEv(KeyPressedEvent& event);
	private:
		Ref<Camera> m_Camera;

		Ref<VertexArray> m_VertexArray;
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;
		Ref<Framebuffer> m_Framebuffer;

		ShaderLibrary shaderLib;
		Ref<Texture2D> m_Texture;

		Ref<Scene> m_Scene;

		std::vector<ProfileResult> m_ProfileResults;

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
