#pragma once

#include "imgui/imgui.h"

#include <functional>

namespace DEngine
{
	class DockspacePanel
	{
	public:
		void OnRender(const std::function<void()>& drawContents);

	private:
		bool m_Open = true;
		bool m_Fullscreen = true;
		bool m_Padding = false;
		ImGuiDockNodeFlags m_Flags = ImGuiDockNodeFlags_None;
	};
}
