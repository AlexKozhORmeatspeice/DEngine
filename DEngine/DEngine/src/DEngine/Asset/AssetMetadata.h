#pragma once

#include "Asset.h"
#include <filesystem>

namespace DEngine
{
	struct AssetMetadata
	{
		AssetType Type = AssetType::None;
		std::filesystem::path FilePath;
	};
}
