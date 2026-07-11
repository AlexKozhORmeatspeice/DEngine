#include "dpch.h"
#include "ModelImporter.h"
#include "DEngine/Asset/AssetManager.h"
#include "DEngine/Asset/AssetManagerBase.h"
#include "DEngine/Asset/Importer/TextureImporter.h"

#include "DEngine/Renderer/Mesh/Mesh.h"
#include "DEngine/Renderer/Material/Material.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


namespace DEngine
{
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

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(
            path.string(), 
            aiProcess_CalcTangentSpace |
            aiProcess_GenNormals
        );

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            D_CORE_ERROR("ModelImporter: assimp error: {0}", importer.GetErrorString());
            return nullptr;
        }

        std::vector<MeshRenderData> renderData;
        auto directory = path.parent_path();

        ProcessNode(scene->mRootNode, scene, directory, renderData);

        if (renderData.empty())
        {
            D_CORE_ERROR("ModelImporter: no meshes found in {0}", path.string());
            return nullptr;
        }

        // Создаём модель
        std::string filePath = path.string();

        auto lastSlash = filePath.find_last_of("/\\");
        lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;

        auto lastDot = filePath.rfind(".");
        auto count = lastDot == std::string::npos ? filePath.size() - lastSlash : lastDot - lastSlash;

        std::string name = filePath.substr(lastSlash, count);

        auto model = CreateRef<Model>(renderData, name);
        return model;
    }

    void ModelImporter::ProcessNode(aiNode* node, const aiScene* scene, 
                                    const std::filesystem::path& directory,
                                    std::vector<MeshRenderData>& renderData)
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

            // Извлекаем индексы в CCW порядке
            std::vector<uint32_t> indices;
            indices.reserve(mesh->mNumFaces * 3);

            for (unsigned int f = 0; f < mesh->mNumFaces; f++)
            {
                aiFace face = mesh->mFaces[f];
                // CCW порядок: меняем местами второй и третий индекс
                indices.push_back(face.mIndices[0]);
                indices.push_back(face.mIndices[1]);
                indices.push_back(face.mIndices[2]);
            }

            // Создаём Mesh как ассет
            std::string meshName = "mesh" + std::to_string(i);
            std::filesystem::path meshPath = directory / meshName;

			BufferLayout layout = 
			{
				{ShaderDataType::Float3, "a_Position"},
				{ShaderDataType::Float3, "a_Normal"},
				{ShaderDataType::Float2, "a_Texcoord"},
				{ShaderDataType::Float3, "a_Tangent"},
			};

            const AssetHandle meshHandle = AssetManager::CreateMeshAsset(layout, vertices.data(), vertices.size() * sizeof(float), indices.data(), indices.size(), meshPath);

            // Загружаем материал
            std::string matName = "material" + std::to_string(i);
            std::filesystem::path matPath = directory / matName;
            AssetHandle materialHandle = AssetManager::CreateAsset({AssetType::Material, matPath});

            // Загружаем albedo текстуру в материал
            if (mesh->mMaterialIndex >= 0)
            {
                aiMaterial* aiMat = scene->mMaterials[mesh->mMaterialIndex];
                
                // Получаем Ref<Material>
                Ref<Material> material = AssetManager::GetAsset<Material>(materialHandle);
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

            renderData.push_back({ meshHandle, materialHandle });
        }

        // Рекурсивно обрабатываем дочерние узлы
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(node->mChildren[i], scene, directory, renderData);
        }
    }
}