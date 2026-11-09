#pragma once

#include "DEngine/Core/UUID.h"

namespace DEngine
{
	using AssetHandle = UUID;

	enum class AssetType : uint16_t 
	{
		None = 0,
		Scene,
		Texture2D,
		Mesh,
		Material,
		Model,
		Shader
	};

	class Asset
	{
	public:
		AssetHandle handle; //automaticly generates handle

		virtual AssetType GetType() const = 0;
	};
}
