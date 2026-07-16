#pragma once
#include "DEngine/Core.h"

#include "DEngine/Asset/Asset.h"
#include "DEngine/Asset/AssetMetadata.h"

#include "DEngine/Renderer/Mesh/Model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace DEngine
{
	class ModelImporter
	{
	public:
		static Ref<Model> ImportModel(AssetHandle handle, const AssetMetadata& metadata);
		static Ref<Model> LoadModel(const std::filesystem::path& path);
	private:
        static void ProcessNode(aiNode* node, const aiScene* scene, 
                                const std::filesystem::path& directory,
                                std::vector<MeshRenderData>& renderData,
								const std::string& modelName,
								int nodeID);

		static std::string GetModelName(const std::filesystem::path& path);
		static std::filesystem::path ConstructMeshPath(const std::string& modelName, int meshInd, int nodeInd);
	};
}

