#pragma once
#include "DEngine/Core.h"

#include "DEngine/Asset/Asset.h"
#include "DEngine/Asset/AssetMetadata.h"

#include "DEngine/Renderer/Mesh/Mesh.h"

namespace DEngine
{
	class MeshImporter
	{
	public:
		static Ref<Mesh> ImportMesh(AssetHandle handle, const AssetMetadata& metadata);
		static Ref<Mesh> LoadMesh(const std::filesystem::path& path);
	};
}

