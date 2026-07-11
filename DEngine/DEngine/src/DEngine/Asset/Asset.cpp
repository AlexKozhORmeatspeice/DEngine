#include "dpch.h"
#include "Asset.h"

namespace DEngine
{
	std::string_view AssetTypeToString(AssetType type)
	{
		switch (type)
		{
			case AssetType::None:		return "AssetType::None";
			case AssetType::Scene:		return "AssetType::Scene";
			case AssetType::Texture2D:	return "AssetType::Texture2D";
			case AssetType::Mesh:		return "AssetType::Mesh";
			case AssetType::Material:	return "AssetType::Material";
			case AssetType::Model:		return "AssetType::Model";
			case AssetType::Shader:		return "AssetType::Shader";
		}

		return "AssetType::Invalid";
	}

	AssetType AssetTypeFromString(std::string_view str)
	{
		if (str == "AssetType::None") return AssetType::None;
		if (str == "AssetType::Scene") return AssetType::Scene;
		if (str == "AssetType::Texture2D") return AssetType::Texture2D;
		if (str == "AssetType::Mesh") return AssetType::Mesh;
		if (str == "AssetType::Material") return AssetType::Material;
		if (str == "AssetType::Model") return AssetType::Model;
		if (str == "AssetType::Shader") return AssetType::Shader;

		return AssetType::None;
	}
}