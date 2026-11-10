#include "SceneInfoPanel.h"

#include "DEngine/Profiler/Profiler.h"
#include "imgui/imgui.h"

#include <cstring>
#include <string>

namespace DEngine
{
	void SceneInfoPanel::OnRender(const Ref<Scene>& scene)
	{
		D_PROFILE_FUNCTION();

		ImGui::Begin("Scene info");
		const int objCount = scene ? (int)scene->GetAllEntities().size() : 0;
		char objCountLabel[50];
		strcpy(objCountLabel, std::to_string(objCount).c_str());
		strcat(objCountLabel, " Objects on scene");
		ImGui::Text(objCountLabel);
		ImGui::End();
	}
}
