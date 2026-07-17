#include "dpch.h"
#include "MeshImporter.h"

#include "DEngine/Asset/Serializer/MeshSerializer.h"

namespace DEngine
{
	Ref<Mesh> MeshImporter::ImportMesh(AssetHandle handle, const AssetMetadata& metadata)
	{
		return LoadMesh(metadata.FilePath);
	}

	Ref<Mesh> MeshImporter::LoadMesh(const std::filesystem::path& path)
	{
		DeserializeMeshResult& desRes = MeshSerializer::Deserialize(path);
		Ref<Mesh> resMesh = nullptr;

		if (desRes.isSuccessful)
		{
			resMesh = desRes.mesh;
		}
		else
		{
			resMesh = nullptr;
			D_CORE_ERROR("Couldn't find mesh by path {0}", path.string());
		}

		return resMesh;
	}
}