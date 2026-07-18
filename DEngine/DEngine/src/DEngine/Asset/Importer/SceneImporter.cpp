#include "dpch.h"
#include "SceneImporter.h"

#include "DEngine/Scene/Serialization/SceneSerializer.h"

namespace DEngine
{
	Ref<Scene> SceneImporter::ImportScene(AssetHandle handle, const AssetMetadata& metadata)
	{
		return LoadScene(metadata.FilePath);
	}

	Ref<Scene> SceneImporter::LoadScene(const std::filesystem::path& path)
	{
		DeserializeSceneResult& desRes = SceneSerializer::Deserialize(path);
		Ref<Scene> resScene = nullptr;

		if (desRes.isSuccessful)
		{
			resScene = desRes.scene;
		}
		else
		{
			resScene = nullptr;
			D_CORE_ERROR("Couldn't find scene by path {0}", path.string());
		}

		return resScene;

	}
}