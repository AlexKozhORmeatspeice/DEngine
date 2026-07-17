#pragma once
#include "DEngine/Core.h"
#include "DEngine/Renderer/Mesh/Mesh.h"

#include "filesystem"

namespace DEngine
{
	struct DeserializeMeshResult
	{
		bool isSuccessful;
		Ref<Mesh> mesh;
	};

	class MeshSerializer
	{
	public:
		static void Serialize(const Ref<Mesh>& mesh, const MeshData& data, const std::filesystem::path& path);
		static DeserializeMeshResult Deserialize(const std::filesystem::path& path);
	};
}
