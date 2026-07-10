#include "dpch.h"
#include "MeshImporter.h"

namespace DEngine
{
	Ref<Mesh> MeshImporter::ImportMesh(AssetHandle handle, const AssetMetadata& metadata)
	{
		return LoadMesh(metadata.FilePath);
	}

	Ref<Mesh> MeshImporter::LoadMesh(const std::filesystem::path& path)
	{
		return nullptr;
	}
}