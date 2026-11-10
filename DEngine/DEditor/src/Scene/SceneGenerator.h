#pragma once

#include "DEngine.h"

namespace DEngine
{
	class SceneGenerator
	{
	public:
		static const char* BaseScenePath();
		static Ref<Scene> Create();
	};
}
