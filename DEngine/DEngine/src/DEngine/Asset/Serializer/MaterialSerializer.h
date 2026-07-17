#pragma once
#include "DEngine/Core.h"
#include "DEngine/Renderer/Material/Material.h"

#include "filesystem"

namespace DEngine
{
	struct DeserializeMaterialResult
	{
		bool isSuccessful;
		Ref<Material> material;
	};

	class MaterialSerializer
	{
	public:
		static void Serialize(const Ref<Material>& material, const std::filesystem::path& path);
		static DeserializeMaterialResult Deserialize(const std::filesystem::path& path);
	};
}

