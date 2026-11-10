#pragma once

#include "Editor/GameMode.h"

#include <functional>

namespace DEngine
{
	struct EditorMenuActions
	{
		std::function<void()> Play;
		std::function<void()> Stop;
		std::function<void()> Serialize;
		std::function<void()> Deserialize;
		std::function<void()> Exit;
	};

	class EditorMenuPanel
	{
	public:
		void OnRender(GameMode mode, const EditorMenuActions& actions);
	};
}
