#include "GameViewportPanel.h"

#include "Panels/AssetDragDrop.h"
#include "DEngine/Profiler/Profiler.h"

namespace DEngine
{
	void GameViewportPanel::Init()
	{
		const Window& win = Application::Get().GetWindow();
		m_Framebuffer = Framebuffer::Create({ win.GetWidth(), win.GetHeight() });
		m_Size = { (float)win.GetWidth(), (float)win.GetHeight() };
	}

	void GameViewportPanel::RenderGraphic(const Timestep& ts, const Ref<Scene>& scene, const Ref<Camera>& camera)
	{
		if (!m_Open || !m_Framebuffer || !camera || !scene)
			return;

		const FramebufferSpecification& spec = m_Framebuffer->GetSpecification();
		const uint32_t width = (uint32_t)m_Size.x;
		const uint32_t height = (uint32_t)m_Size.y;
		if (width > 0 && height > 0 && (spec.Width != width || spec.Height != height))
			m_Framebuffer->Resize(width, height);

		Renderer::BeginScene(camera, m_Framebuffer);
		scene->OnRender(ts);
		Renderer::EndScene();
	}

	void GameViewportPanel::OnRender(const Ref<Scene>& scene)
	{
		D_PROFILE_FUNCTION();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		m_Open = ImGui::Begin("Game");
		m_Hovered = m_Open && ImGui::IsWindowHovered();

		if (m_Open)
		{
			const ImVec2 gameSize = ImGui::GetContentRegionAvail();
			if (gameSize.x > 0.0f && gameSize.y > 0.0f && (m_Size.x != gameSize.x || m_Size.y != gameSize.y))
				m_Size = { gameSize.x, gameSize.y };

			if (m_Framebuffer)
				ImGui::Image(m_Framebuffer->GetImGuiHandle(), ImVec2{ gameSize.x, gameSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
			AcceptModelDrop(scene);
			if (!m_UsesSceneCamera)
			{
				ImGui::SetCursorPos(ImVec2(8.0f, 8.0f));
				ImGui::TextDisabled("No scene camera");
			}
		}

		ImGui::End();
		ImGui::PopStyleVar();
	}
}
