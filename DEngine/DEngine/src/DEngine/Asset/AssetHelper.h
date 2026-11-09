#pragma once

#include "DEngine/Asset/Asset.h"

#include <filesystem>
#include <string>
#include <string_view>

namespace DEngine
{
	class AssetHelper
	{
	public:
		static AssetType TypeFromExtension(const std::filesystem::path& path);
		static std::string_view NativeExtension(AssetType type);

		static std::string_view ToString(AssetType type);
		static AssetType FromString(std::string_view str);

		static std::filesystem::path NormalizePath(const std::filesystem::path& path);
		static bool SamePath(const std::filesystem::path& a, const std::filesystem::path& b);
		static bool IsResourcesPath(const std::filesystem::path& path);
		static void EnsureParentDirectory(const std::filesystem::path& path);

		static std::string DisplayName(AssetHandle handle);
	};
}
