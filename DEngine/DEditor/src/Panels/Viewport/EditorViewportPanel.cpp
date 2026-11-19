#include "EditorViewportPanel.h"

#include "Panels/Properties/PropertiesPanel.h"
#include "Panels/AssetDragDrop.h"
#include "DEngine/Profiler/Profiler.h"

#include <glm/gtc/type_ptr.hpp>

namespace DEngine
{
	void EditorViewportPanel::Init(const Ref<PerspectiveCamera>& camera)
	{
		m_Camera = camera;
		const Window& win = Application::Get().GetWindow();
		m_Framebuffer = Framebuffer::Create({ win.GetWidth(), win.GetHeight() });
		m_Size = { (float)win.GetWidth(), (float)win.GetHeight() };
	}

	void EditorViewportPanel::OnUpdate(const Timestep& ts)
	{
		(void)ts;
		if (Input::IsKeyPressed(D_KEY_TAB))
		{
			if (!m_TabJustPressed)
			{
				m_TabJustPressed = true;
				m_GuizmoType = m_GuizmoType == ImGuizmo::OPERATION::TRANSLATE ? ImGuizmo::OPERATION::ROTATE
					: m_GuizmoType == ImGuizmo::OPERATION::ROTATE ? ImGuizmo::OPERATION::SCALE
					: ImGuizmo::OPERATION::TRANSLATE;
			}
		}
		else
		{
			m_TabJustPressed = false;
		}
	}

	void EditorViewportPanel::RenderGraphic(const Timestep& ts, const Ref<Scene>& scene)
	{
		if (!m_Open || !m_Framebuffer || !m_Camera || !scene)
			return;

		const FramebufferSpecification& spec = m_Framebuffer->GetSpecification();
		const uint32_t width = (uint32_t)m_Size.x;
		const uint32_t height = (uint32_t)m_Size.y;
		if (width > 0 && height > 0 && (spec.Width != width || spec.Height != height))
		{
			m_Framebuffer->Resize(width, height);
			m_Camera->ChangeSize(width, height);
		}

		Renderer::BeginScene(m_Camera, m_Framebuffer);
		scene->OnRender(ts);
		Renderer::EndScene();
	}

	void EditorViewportPanel::OnRender(const Ref<Scene>& scene, PropetiesPanel& properties)
	{
		D_PROFILE_FUNCTION();
		ImGuizmo::BeginFrame();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		m_Open = ImGui::Begin("Editor");
		m_Focused = m_Open && ImGui::IsWindowFocused();
		m_Hovered = m_Open && ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->SetBlockEvents(!m_Focused || !m_Hovered);

		if (m_Open)
		{
			const ImVec2 editorSize = ImGui::GetContentRegionAvail();
			if (editorSize.x > 0.0f && editorSize.y > 0.0f && (m_Size.x != editorSize.x || m_Size.y != editorSize.y))
				m_Size = { editorSize.x, editorSize.y };

			if (m_Framebuffer)
				ImGui::Image(m_Framebuffer->GetImGuiHandle(), ImVec2{ editorSize.x, editorSize.y });
			AcceptModelDrop(scene);
			DrawGizmo(properties);
		}

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void EditorViewportPanel::DrawGizmo(PropetiesPanel& properties)
	{
		Entity selected = properties.getSelectedEntity();
		if (!selected || !selected.HasComponent<TransformComponent>() || !m_Camera)
			return;

		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();
		const ImVec2 winpos = ImGui::GetWindowPos();
		ImGuizmo::SetRect(winpos.x, winpos.y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

		const glm::mat4 camViewMat = m_Camera->GetViewMat();
		const glm::mat4 camProjMat = m_Camera->GetProjMat();
		TransformComponent& transform = selected.GetComponent<TransformComponent>();
		glm::mat4& entityTransform = transform.GetModelMatrixLink();
		if (ImGuizmo::Manipulate(glm::value_ptr(camViewMat), glm::value_ptr(camProjMat),
			m_GuizmoType, ImGuizmo::WORLD, glm::value_ptr(entityTransform)))
			transform.isChangedTransform = true;
	}
}
