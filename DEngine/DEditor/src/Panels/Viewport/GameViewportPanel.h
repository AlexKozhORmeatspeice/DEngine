#pragma once

#include "DEngine.h"

namespace DEngine
{
	class GameViewportPanel
	{
	public:
		void Init();
		void OnRender(const Ref<Scene>& scene);
		void RenderGraphic(const Timestep& ts, const Ref<Scene>& scene, const Ref<Camera>& camera);
		void SetUsesSceneCamera(bool value) { m_UsesSceneCamera = value; }

		glm::vec2 GetSize() const { return m_Size; }
		bool IsOpen() const { return m_Open; }
		bool IsHovered() const { return m_Hovered; }

	private:
		Ref<Framebuffer> m_Framebuffer;
		glm::vec2 m_Size{ 0.0f, 0.0f };
		bool m_Open = false;
		bool m_Hovered = false;
		bool m_UsesSceneCamera = false;
	};
}
