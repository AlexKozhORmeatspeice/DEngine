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

		m_Framebuffer->Bind();
		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.15f, 1.0f });
		RenderCommand::Clear();
		Renderer::BeginScene(m_Camera);
		scene->OnRender(ts);
		Renderer::EndScene();
		m_Framebuffer->Unbind();
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
			{
				m_Size = { editorSize.x, editorSize.y };
				m_Framebuffer->Resize((uint32_t)editorSize.x, (uint32_t)editorSize.y);
				m_Camera->ChangeSize((uint32_t)editorSize.x, (uint32_t)editorSize.y);
			}

			ImGui::Image((void*)m_Framebuffer->GetColorAttachmentRendererID(), ImVec2{ editorSize.x, editorSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
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
		glm::mat4& entityTransform = selected.GetComponent<TransformComponent>().GetModelMatrixLink();
		ImGuizmo::Manipulate(glm::value_ptr(camViewMat), glm::value_ptr(camProjMat),
			m_GuizmoType, ImGuizmo::WORLD, glm::value_ptr(entityTransform));
	}
}
