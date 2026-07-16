#include "dpch.h"
#include "EditorAssetManager.h"
#include "Importer/AssetImporter.h"

#include "DEngine/Project/Project.h"
#include "DEngine/Asset/FileWatcher.h"

#include "DEngine/Renderer/Shader/Shader.h"
#include "DEngine/Renderer/Mesh/Mesh.h"
#include "DEngine/Renderer/Texture/Texture.h"
#include "DEngine/Asset/Asset.h"
#include "DEngine/Asset/Serializer/MeshSerializer.h"

#include "fstream"
#include "yaml-cpp/yaml.h"

namespace DEngine
{
	const std::string MESHES_DIR_NAME = "meshes";

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
		SetupFileWatcher();
	}

	void EditorAssetManager::InitAssetDir()
	{
		
	}

	void EditorAssetManager::Update()
	{
		if (m_HotReloadEnabled)
		{
			m_FileWatcher.Update();
		}
	}

	void EditorAssetManager::ReloadAsset(const AssetHandle& handle)
	{
		const auto& metadata = GetMetadata(handle);
		if (metadata.Type == AssetType::None)
		{
			return;
		}

		// Проверяем, существует ли файл
		if (!metadata.FilePath.empty() && !std::filesystem::exists(metadata.FilePath))
		{
			D_CORE_WARN("Hot reload: File {0} doesn't exist", metadata.FilePath.string());
			return;
		}

		// Импортируем заново
		auto newAsset = AsssetImporter::ImportAsset(handle, metadata);
		if (!newAsset)
		{
			D_CORE_ERROR("Hot reload: Failed to reload asset from {0}", metadata.FilePath.string());
			return;
		}

		// Заменяем в кеше
		m_LoadedAssets[handle] = newAsset;

		// Уведомляем системы об обновлении
		if (metadata.Type == AssetType::Shader)
		{
			ReloadShader(handle, metadata);
		}
		else if (metadata.Type == AssetType::Mesh)
		{
			ReloadMesh(handle, metadata);
		}
		else if (metadata.Type == AssetType::Texture2D)
		{
			ReloadTexture(handle, metadata);
		}

		D_CORE_INFO("Hot reload: {0} reloaded successfully", AssetTypeToString(metadata.Type));
	}

	void EditorAssetManager::ReloadShader(const AssetHandle& handle, const AssetMetadata& metadata)
	{
		auto shader = std::dynamic_pointer_cast<Shader>(m_LoadedAssets[handle]);
		if (shader)
		{
			Ref<Asset> asset = AsssetImporter::ImportAsset(handle, metadata);
			m_LoadedAssets[handle] = asset;
		}
	}

	void EditorAssetManager::ReloadMesh(const AssetHandle& handle, const AssetMetadata& metadata)
	{
		auto mesh = std::dynamic_pointer_cast<Mesh>(m_LoadedAssets[handle]);
		if (mesh)
		{
			Ref<Asset> asset = AsssetImporter::ImportAsset(handle, metadata);
			m_LoadedAssets[handle] = asset;
		}
	}

	void EditorAssetManager::ReloadTexture(const AssetHandle& handle, const AssetMetadata& metadata)
	{
		auto texture = std::dynamic_pointer_cast<Texture2D>(m_LoadedAssets[handle]);
		if (texture)
		{
			Ref<Asset> asset = AsssetImporter::ImportAsset(handle, metadata);
			m_LoadedAssets[handle] = asset;
		}
	}

	void EditorAssetManager::ReloadAssetByPath(const std::filesystem::path& path)
	{
		auto it = m_PathToHandle.find(path);
		if (it != m_PathToHandle.end())
		{
			ReloadAsset(it->second);
		}
		else
		{
			D_CORE_WARN("Hot reload: No asset registered for path {0}", path.string());
		}
	}

	const AssetHandle& EditorAssetManager::CreateAsset(const std::filesystem::path& path)
	{
		AssetType type = AssetType::None;

		// Определяем тип ресурса по расширению файла
		std::string extension = path.extension().string();
		
		if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" || 
			extension == ".bmp" || extension == ".tga" || extension == ".psd")
		{
			type = AssetType::Texture2D;
		}
		else if (extension == ".obj" || extension == ".fbx" || extension == ".gltf" || 
				 extension == ".glb" || extension == ".3ds" || extension == ".dae")
		{
			type = AssetType::Model;
		}
		else if (extension == ".glsl" || extension == ".vert" || extension == ".frag" || 
				 extension == ".comp" || extension == ".geom" || extension == ".tesc" || 
				 extension == ".tese")
		{
			type = AssetType::Shader;
		}
		else
		{
			D_CORE_ERROR("EditorAssetManager::CreateAsset: unsupported file type: {0}", extension);
			type = AssetType::None;
		}

		//TODO: дописать для остальных типов ресурсов, когда они будут готовы

		AssetHandle handle = CreateAsset({ type, path });

		return handle;
	}

	const AssetHandle& EditorAssetManager::CreateAsset(AssetMetadata metadata)
	{
		if (metadata.Type == AssetType::None) return AssetHandle::Invalid();

		AssetHandle handle;
		m_AssetRegistry[handle] = metadata;

		// Добавляем файл в watcher если путь указан
		if (!metadata.FilePath.empty() && std::filesystem::exists(metadata.FilePath) && metadata.Type != AssetType::Mesh)
		{
			m_PathToHandle[metadata.FilePath] = handle;
			m_HandleToPath[handle] = metadata.FilePath;
			
			m_FileWatcher.AddWatchPath(metadata.FilePath, [this](const std::filesystem::path& changedPath) {
				OnFileChanged(changedPath);
			});
		}

		return handle;
	}

	const AssetHandle& EditorAssetManager::CreateMeshAsset(const BufferLayout& layout,
														   float* verts, uint32_t vertsSize,
														   uint32_t* inds, uint32_t indsSize,
														   const std::filesystem::path& path)
	{
		AssetHandle handle;
		Ref<Mesh> mesh = nullptr;

		//Пробуем десериализовать существующий файл мэша
		const DeserializeMeshResult& deserMeshRes = MeshSerializer::Deserialize(path);
		if (deserMeshRes.isSuccessful)
		{
			handle = deserMeshRes.handle;
			mesh = deserMeshRes.mesh;
		}
		else // Если файла еще не существует - создаем и сериализуем
		{
			MeshData meshData;

			meshData.verts = verts;
			meshData.vertSize = vertsSize;
			meshData.inds = inds;
			meshData.indsSize = indsSize;

			mesh = CreateRef<Mesh>(layout, meshData);

			MeshSerializer::Serialize(handle, mesh, meshData, path);
		}

		m_AssetRegistry[handle] = { AssetType::Mesh, path };
		m_LoadedAssets[handle] = mesh;

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

			// Сохраняем время загрузки для отслеживания изменений
			if (!metadata.FilePath.empty() && std::filesystem::exists(metadata.FilePath))
			{
				m_AssetLastLoadTime[handle] = std::filesystem::last_write_time(metadata.FilePath);
			}
		}

		asset = m_LoadedAssets[handle];

		return asset;
	}

	bool EditorAssetManager::IsAssetHandleValid(const AssetHandle& handle) const
	{
		if (handle == 0) return false;

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

			// Добавляем в FileWatcher при десериализации
			if (!metadata.FilePath.empty() && std::filesystem::exists(metadata.FilePath))
			{
				m_PathToHandle[metadata.FilePath] = handle;
				m_HandleToPath[handle] = metadata.FilePath;
				
				m_FileWatcher.AddWatchPath(metadata.FilePath, [this](const std::filesystem::path& changedPath) {
					OnFileChanged(changedPath);
				});
			}
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
		Ref<Mesh> mesh;

		std::filesystem::path meshPrimitivePath = MeshGenerator::ConstructPrimitivePath(type);
		const DeserializeMeshResult& deserRes = MeshSerializer::Deserialize(meshPrimitivePath);

		if (deserRes.isSuccessful)
		{
			handle = deserRes.handle;
			mesh = deserRes.mesh;
		}
		else
		{
			mesh = MeshGenerator::CreatePrimitive(type);
		}

		m_AssetRegistry[handle] = { AssetType::Mesh, meshPrimitivePath };
		m_LoadedAssets[handle] = mesh;
		m_MeshPrimitives[type] = handle;

		return handle;
	}

	void EditorAssetManager::SetupFileWatcher()
	{
		auto assetsPath = Project::GetAssetRegistryPath();
		
		if (!std::filesystem::exists(assetsPath))
		{
			D_CORE_WARN("EditorAssetManager: Assets directory {0} doesn't exist", assetsPath.string());
			return;
		}

		// Рекурсивно добавляем все существующие файлы в watch
		for (const auto& entry : std::filesystem::recursive_directory_iterator(assetsPath))
		{
			if (entry.is_regular_file())
			{
				auto path = entry.path();
				m_FileWatcher.AddWatchPath(path, [this](const std::filesystem::path& changedPath) {
					OnFileChanged(changedPath);
				});
			}
		}

		D_CORE_INFO("EditorAssetManager: FileWatcher setup complete, watching {0}", assetsPath.string());
	}

	void EditorAssetManager::OnFileChanged(const std::filesystem::path& path)
	{
		if (!m_HotReloadEnabled)
			return;

		// Проверяем, зарегистрирован ли этот файл
		auto it = m_PathToHandle.find(path);
		if (it != m_PathToHandle.end())
		{
			AssetHandle handle = it->second;
			
			// Проверяем, не загружен ли ассет в данный момент
			auto loadIt = m_AssetLastLoadTime.find(handle);
			if (loadIt != m_AssetLastLoadTime.end())
			{
				// Если файл был изменен во время загрузки, пропускаем
				auto currentWrite = std::filesystem::last_write_time(path);
				if (currentWrite <= loadIt->second)
					return;
			}

			D_CORE_INFO("Hot reload: File {0} changed, reloading asset", path.string());
			ReloadAsset(handle);
			
			// Обновляем время загрузки
			m_AssetLastLoadTime[handle] = std::filesystem::last_write_time(path);
		}
	}
}