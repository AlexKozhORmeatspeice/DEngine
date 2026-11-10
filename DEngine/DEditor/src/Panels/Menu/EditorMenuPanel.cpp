#include "EditorMenuPanel.h"

#include "imgui/imgui.h"
#include "DEngine/Profiler/Profiler.h"

namespace DEngine
{
	void EditorMenuPanel::OnRender(GameMode mode, const EditorMenuActions& actions)
	{
		D_PROFILE_FUNCTION();

		if (!ImGui::BeginMenuBar())
			return;

		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Serialize") && actions.Serialize)
				actions.Serialize();
			if (ImGui::MenuItem("Deserialize") && actions.Deserialize)
				actions.Deserialize();
			if (ImGui::MenuItem("Exit") && actions.Exit)
				actions.Exit();
			ImGui::EndMenu();
		}

		ImGui::SameLine();
		if (mode == GameMode::EDITOR)
		{
			if (ImGui::Button("Play") && actions.Play)
				actions.Play();
		}
		else if (ImGui::Button("Stop") && actions.Stop)
		{
			actions.Stop();
		}

		ImGui::EndMenuBar();
	}
}
