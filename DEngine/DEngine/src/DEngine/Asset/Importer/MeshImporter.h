#pragma once
#include "DEngine/Core.h"

#include "DEngine/Asset/Asset.h"
#include "DEngine/Asset/AssetMetadata.h"

#include "DEngine/Renderer/Mesh/Mesh.h"

namespace DEngine
{
	struct DecodedMesh
	{
		BufferLayout Layout;
		MeshData Data;
		bool Succeeded = false;
	};

	class MeshImporter
	{
	public:
		static Ref<Mesh> ImportMesh(AssetHandle handle, const AssetMetadata& metadata);
		static Ref<Mesh> LoadMesh(const std::filesystem::path& path);
		static DecodedMesh DecodeMesh(const std::filesystem::path& path);
		static Ref<Mesh> UploadMesh(const BufferLayout& layout, MeshData& data);
	};
}

