#pragma once
#include "DEngine/Core.h"
#include "DEngine/Renderer/Material/Material.h"
#include "DEngine/Renderer/Mesh/Model.h"

#include "filesystem"

namespace DEngine
{
	struct DeserializeModelResult
	{
		bool isSuccessful;
		Ref<Model> model;
	};

	class ModelSerializer
	{
	public:
		static void Serialize(const Ref<Model>& model, const std::filesystem::path& path);
		static DeserializeModelResult Deserialize(const std::filesystem::path& path);
	};
}
