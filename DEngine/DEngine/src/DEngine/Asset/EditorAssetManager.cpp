#include "dpch.h"
#include "EditorAssetManager.h"
#include "Importer/AssetImporter.h"

#include "DEngine/Project/Project.h"

#include "fstream"
#include "yaml-cpp/yaml.h"

namespace DEngine
{
	const std::string HANDLE_NAME = "Handle";
	const std::string FILEPATH_NAME = "Filepath";
	const std::string TYPE_NAME = "Type";

	//Добавли поддержку string_view
	YAML::Emitter& operator<<(YAML::Emitter& out, const std::string_view& v)
	{
		out << std::string(v.data(), v.size());
		return out;
	}

	EditorAssetManager::EditorAssetManager()
	{
		CreateBaseRendererShader();
	}

	const AssetHandle& EditorAssetManager::CreateAsset(AssetMetadata metadata)
	{
		AssetHandle handle;
		m_AssetRegistry[handle] = metadata;

		return handle;
	}

	const AssetHandle& EditorAssetManager::CreateMeshAsset(const BufferLayout& layout,
														   float* verts, uint32_t vertsSize,
														   uint32_t* inds, uint32_t indsSize,
														   const std::filesystem::path& path)
	{
		AssetHandle handle;
		m_AssetRegistry[handle] = { AssetType::Mesh, path};
		m_LoadedAssets[handle] = CreateRef<Mesh>(layout, verts, vertsSize, inds, indsSize);

		return handle;
	}

	Ref<Asset> EditorAssetManager::GetAsset(const AssetHandle& handle)
	{
		const auto& metadata = GetMetadata(handle);

		if (metadata.Type == AssetType::None)
			return nullptr;
			
		Ref<Asset> asset;
		if (!IsAssetLoaded(handle))
		{
			asset = AsssetImporter::ImportAsset(handle, metadata);
			if (!asset) 
			{
				D_CORE_ASSERT(false, "EditorAssetManager::GetAsset: couldn't load {0} by path {1}", AssetTypeToString(metadata.Type), metadata.FilePath.string());
				return nullptr;
			}

			m_LoadedAssets[handle] = asset;
		}

		asset = m_LoadedAssets[handle];

		return asset;
	}

	bool EditorAssetManager::IsAssetHandleValid(const AssetHandle& handle) const
	{
		if (handle == 0) return 0;

		return m_AssetRegistry.find(handle) != m_AssetRegistry.end();
	}

	bool EditorAssetManager::IsAssetLoaded(const AssetHandle& handle) const
	{
		return m_LoadedAssets.find(handle) != m_LoadedAssets.end();
	}

	const AssetMetadata& EditorAssetManager::GetMetadata(const AssetHandle& handle)
	{
		static AssetMetadata s_NullMetadata;

		auto it = m_AssetRegistry.find(handle);
		if (it == m_AssetRegistry.end())
		{
			return s_NullMetadata;
		}

		return it->second;
	}

	const AssetHandle& EditorAssetManager::GetPrimitiveMesh(PrimitiveType type)
	{
		auto it = m_MeshPrimitives.find(type);
		if (it != m_MeshPrimitives.end())
			return it->second;

		return CreateMeshPrimitive(type);
	}
	
	void EditorAssetManager::SerializeAssetRegistry()
	{
		auto path = Project::GetAssetRegistryPath();

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "AssetRegistry" << YAML::Value;
		out << YAML::BeginSeq;

		for (const auto& [handle, metadata] : m_AssetRegistry)
		{
			out << YAML::BeginMap;
			out << YAML::Key << HANDLE_NAME << YAML::Value << handle;
			out << YAML::Key << FILEPATH_NAME << YAML::Value << metadata.FilePath.string();
			out << YAML::Key << TYPE_NAME << YAML::Value << AssetTypeToString(metadata.Type);
			out << YAML::EndMap;
		}

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(path);
		fout << out.c_str();
		fout.close();
	}

	bool EditorAssetManager::DeserializeAssetRegistry()
	{
		auto path = Project::GetAssetRegistryPath();

		YAML::Node data;
		try
		{
			data = YAML::LoadFile(path.string());
		}
		catch (YAML::ParserException e)
		{
			D_CORE_ERROR("Couldn't load file {0}", path.string());
			return false;
		}

		auto rootNode = data["AssetRegistry"];
		if (!rootNode)
			return false;

		for (const auto& node : rootNode)
		{
			AssetHandle handle = node[HANDLE_NAME].as<uint64_t>();
			auto& metadata = m_AssetRegistry[handle];

			metadata.FilePath = node[FILEPATH_NAME].as<std::string>();
			metadata.Type = AssetTypeFromString(node[TYPE_NAME].as<std::string>());
		}

		return true;
	}

	void EditorAssetManager::CreateBaseRendererShader()
	{
		m_BaseShaderHandle = CreateAsset({AssetType::Shader, "assets/shaders/Base.glsl"});
	}

	const AssetHandle& EditorAssetManager::CreateMeshPrimitive(PrimitiveType type)
	{
		AssetHandle handle;
		m_AssetRegistry[handle] = { AssetType::Mesh, "" };
		m_LoadedAssets[handle] = MeshGenerator::CreatePrimitive(type);

		return handle;
	}
}