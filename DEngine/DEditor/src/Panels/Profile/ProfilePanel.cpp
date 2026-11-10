#include "ProfilePanel.h"

#include "DEngine.h"
#include "DEngine/Profiler/Profiler.h"
#include "imgui/imgui.h"

#include <cstring>
#include <string>

namespace DEngine
{
	void ProfilePanel::OnRender(float frameSeconds)
	{
		D_PROFILE_FUNCTION();

		ImGui::Begin("Profile data");
		char fpsLabel[50];
		char spfLabel[50];
		const int fps = (int)Application::Get().GetFPS();
		strcpy(fpsLabel, std::to_string(fps).c_str());
		strcpy(spfLabel, std::to_string(frameSeconds).c_str());
		strcat(fpsLabel, " FPS");
		strcat(spfLabel, " SPF");
		ImGui::Text(fpsLabel);
		ImGui::Text(spfLabel);
		ImGui::End();
	}
}
