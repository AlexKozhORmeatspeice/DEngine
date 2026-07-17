#include "dpch.h"
#include "ModelImporter.h"
#include "DEngine/Asset/AssetManager.h"
#include "DEngine/Asset/AssetManagerBase.h"
#include "DEngine/Asset/Importer/TextureImporter.h"

#include "DEngine/Renderer/Mesh/Mesh.h"
#include "DEngine/Renderer/Material/Material.h"
#include "DEngine/Project/Project.h"

#include "DEngine/Asset/Serializer/ModelSerializer.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


namespace DEngine
{
    const std::string MODELS_DIR_NAME = "models";
    const std::string MESHES_DIR_NAME = "meshes";
    const std::string MATERIALS_DIR_NAME = "materials";
    const std::string MESH_FILE_NAME = "mesh";

    Ref<Model> ModelImporter::ImportModel(AssetHandle handle, const AssetMetadata& metadata)
    {
        return LoadModel(metadata.FilePath);
    }

    Ref<Model> ModelImporter::LoadModel(const std::filesystem::path& path)
    {
        if (!std::filesystem::exists(path))
        {
            D_CORE_ERROR("ModelImporter: file not found: {0}", path.string());
            return nullptr;
        }

        std::string name = GetModelName(path);
        std::filesystem::path modelPath = ConstructModelPath(name, path);

        //Пробуем десериализовать существующий ресурс
        DeserializeModelResult& deserRes = ModelSerializer::Deserialize(modelPath);
        if (deserRes.isSuccessful)
        {
            return deserRes.model;
        }

        //Если ресурс не был найден - конструируем данные через assimp
        const std::vector<MeshRenderData>& renderData = CreateDataFromAsimp(path);

        if (renderData.empty())
        {
            D_CORE_ERROR("ModelImporter: no meshes found in {0}", path.string());
            return nullptr;
        }

        //Если данные были получены - сериализуем наши ресурсы и загружаем в ассеты
        auto model = CreateRef<Model>(renderData, name);
        AssetManager::CreateModelAsset(model, modelPath);

        return model;
    }

    const std::vector<MeshRenderData> ModelImporter::CreateDataFromAsimp(const std::filesystem::path& path)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(
            path.string(), 
            aiProcess_CalcTangentSpace |
            aiProcess_GenNormals
        );

        std::string name = GetModelName(path);
        std::vector<MeshRenderData> renderData;

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            D_CORE_ERROR("ModelImporter: assimp error: {0}", importer.GetErrorString());
            return renderData;
        }

        auto directory = path.parent_path();

        ProcessNode(scene->mRootNode, scene, directory, renderData, name, 0);

        return std::move(renderData);
    }

    void ModelImporter::ProcessNode(aiNode* node, const aiScene* scene,
									const std::filesystem::path& directory,
									std::vector<MeshRenderData>& renderData,
									const std::string& modelName,
                                    int nodeID)
    {
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            if (!mesh)
                continue;

            std::vector<float> vertices;
            vertices.reserve(mesh->mNumVertices * 11); //11 - количество данных на 1 вертекс

            // Извлекаем вершины
            for (unsigned int v = 0; v < mesh->mNumVertices; v++)
            {
                vertices.push_back(mesh->mVertices[v].x);
                vertices.push_back(mesh->mVertices[v].y);
                vertices.push_back(mesh->mVertices[v].z);
                
                if (mesh->mNormals)
                {
                    vertices.push_back(mesh->mNormals[v].x);
                    vertices.push_back(mesh->mNormals[v].y);
                    vertices.push_back(mesh->mNormals[v].z);
                }
                else
                {
                    vertices.push_back(0.0f);
                    vertices.push_back(0.0f);
                    vertices.push_back(0.0f);
                }

                if (mesh->mTextureCoords[0])
                {
                    vertices.push_back(mesh->mTextureCoords[0][v].x);
                    vertices.push_back(mesh->mTextureCoords[0][v].y);
                }
                else
                {
                    vertices.push_back(0.0f);
                    vertices.push_back(0.0f);
                }

                if (mesh->mTangents)
                {
                    vertices.push_back(mesh->mTangents[v].x);
                    vertices.push_back(mesh->mTangents[v].y);
                    vertices.push_back(mesh->mTangents[v].z);
                }
                else
                {
                    vertices.push_back(0.0f);
                    vertices.push_back(0.0f);
                    vertices.push_back(0.0f);
                }
            }

            std::vector<uint32_t> indices;
            indices.reserve(mesh->mNumFaces * 3);

            for (unsigned int f = 0; f < mesh->mNumFaces; f++)
            {
                aiFace face = mesh->mFaces[f];
                indices.push_back(face.mIndices[0]);
                indices.push_back(face.mIndices[1]);
                indices.push_back(face.mIndices[2]);
            }

            // Создаём Mesh как ассет
			BufferLayout layout = 
			{
				{ShaderDataType::Float3, "a_Position"},
				{ShaderDataType::Float3, "a_Normal"},
				{ShaderDataType::Float2, "a_Texcoord"},
				{ShaderDataType::Float3, "a_Tangent"},
			};

            std::filesystem::path meshPath = ConstructMeshPath(modelName, i, nodeID);
            const AssetHandle meshHandle = AssetManager::CreateMeshAsset(layout, vertices.data(), vertices.size() * sizeof(float), indices.data(), indices.size(), meshPath);

            // Загружаем материал
            AssetHandle baseShaderHandle = AssetManager::GetBaseRendererShaderHandle();
            Ref<Material> material = CreateRef<Material>(baseShaderHandle);

            // Загружаем albedo текстуру в материал
            if (mesh->mMaterialIndex >= 0)
            {
                aiMaterial* aiMat = scene->mMaterials[mesh->mMaterialIndex];
                
                if (material)
                {
                    aiString aiPath;
                    if (aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &aiPath) == AI_SUCCESS)
                    {
                        std::filesystem::path texturePath = directory / aiPath.C_Str();
                        const AssetHandle textureHandle = AssetManager::CreateAsset({AssetType::Texture2D, texturePath});
                        if (textureHandle)
                        {
                            material->SetTexture2D("u_Texture", textureHandle);
                        }
                    }
                }
            }

            //Инитим материал в ресурс
            std::filesystem::path matPath = ConstructMaterialPath(modelName, i, nodeID);
            AssetHandle materialHandle = AssetManager::CreateMaterialAsset(material, matPath);

            renderData.push_back({ meshHandle, materialHandle });
        }

        // Рекурсивно обрабатываем дочерние узлы
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(node->mChildren[i], scene, directory, renderData, modelName, i + 1);
        }
    }

    std::string ModelImporter::GetModelName(const std::filesystem::path& path)
    {
        std::string filePath = path.string();

        auto lastSlash = filePath.find_last_of("/\\");
        lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;

        auto lastDot = filePath.rfind(".");
        auto count = lastDot == std::string::npos ? filePath.size() - lastSlash : lastDot - lastSlash;

        std::string name = filePath.substr(lastSlash, count);
        return name;
    }

    std::filesystem::path ModelImporter::ConstructModelPath(const std::string& modelName, const std::filesystem::path& path)
    {
        std::string name = modelName + DMODEl_FILE_EXT;

        return path / name;
    }

    std::filesystem::path ModelImporter::ConstructMeshPath(const std::string& modelName, int meshInd, int nodeInd)
    {
		std::string meshName = MESH_FILE_NAME + "_" + modelName + "_node_" + std::to_string(nodeInd) + "_mesh_" + std::to_string(meshInd) + DMESH_FILE_EXT;
		std::filesystem::path meshPath = Project::GetResourcesRegistryPath() / MODELS_DIR_NAME / modelName / MESHES_DIR_NAME;
		if (!std::filesystem::exists(meshPath))
		{
			std::filesystem::create_directories(meshPath);
			D_CORE_INFO("Created directory: {0}", meshPath.string());
		}
		meshPath = meshPath / meshName;
        return meshPath;
    }

    std::filesystem::path ModelImporter::ConstructMaterialPath(const std::string& modelName, int materialInd, int nodeInd)
    {
		std::string meshName = MESH_FILE_NAME + "_" + modelName + "_node_" + std::to_string(nodeInd) + "_material_" + std::to_string(materialInd) + DMAT_FILE_EXT;
		std::filesystem::path meshPath = Project::GetResourcesRegistryPath() / MODELS_DIR_NAME / modelName / MATERIALS_DIR_NAME;
		if (!std::filesystem::exists(meshPath))
		{
			std::filesystem::create_directories(meshPath);
			D_CORE_INFO("Created directory: {0}", meshPath.string());
		}
		meshPath = meshPath / meshName;
        return meshPath;

    }
}