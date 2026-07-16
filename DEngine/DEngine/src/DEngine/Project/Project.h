#pragma once

#include "string"
#include "filesystem"

#include "DEngine/Core.h"

namespace DEngine
{
	//Класс для будущей поддержки разных проектов движка. Пока что просто написан для предоставления возможного API
	//Однако, сам функционал нужно еще написать
	class Project
	{
	public:
		//TODO: в будущем должна вытаскивать путь для определенного проекта, а не просто assets
		static std::filesystem::path GetAssetRegistryPath()
		{
			std::filesystem::path exePath = std::filesystem::current_path();
			
			return exePath / "assets";
		}

		static std::filesystem::path GetResourcesRegistryPath()
		{
			std::filesystem::path exePath = std::filesystem::current_path();
			
			return exePath / "resources";
		}
	};
}
