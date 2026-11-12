#include "dpch.h"
#include "MeshImporter.h"

#include "DEngine/Asset/Serializer/MeshSerializer.h"
#include "DEngine/Jobs/JobSystem.h"

namespace DEngine
{
	namespace
	{
		Ref<Mesh> CreateGpuMesh(const BufferLayout& layout, const MeshData& data)
		{
			return CreateRef<Mesh>(layout, data);
		}
	}

	Ref<Mesh> MeshImporter::ImportMesh(AssetHandle handle, const AssetMetadata& metadata)
	{
		return LoadMesh(metadata.FilePath);
	}

	Ref<Mesh> MeshImporter::LoadMesh(const std::filesystem::path& path)
	{
		DecodedMesh decoded = DecodeMesh(path);
		if (!decoded.Succeeded)
		{
			D_CORE_ERROR("Couldn't find mesh by path {0}", path.string());
			return nullptr;
		}

		return UploadMesh(decoded.Layout, decoded.Data);
	}

	DecodedMesh MeshImporter::DecodeMesh(const std::filesystem::path& path)
	{
		DecodedMesh decoded;
		MeshFile file = MeshSerializer::Read(path);
		if (!file.Succeeded)
			return decoded;

		decoded.Layout = std::move(file.Layout);
		decoded.Data = file.Data;
		file.Data.verts = nullptr;
		file.Data.inds = nullptr;
		decoded.Succeeded = true;
		return decoded;
	}

	Ref<Mesh> MeshImporter::UploadMesh(const BufferLayout& layout, MeshData& data)
	{
		const bool onMainThread = !JobSystem::IsInitialized() || JobSystem::ThreadIndex() == 0;

		Ref<Mesh> mesh;
		if (onMainThread)
		{
			mesh = CreateGpuMesh(layout, data);
		}
		else
		{
			PinnedJob upload([&]()
			{
				mesh = CreateGpuMesh(layout, data);
			});
			JobSystem::Schedule(upload);
			JobSystem::Wait(upload);
		}

		data.Release();
		return mesh;
	}
}
