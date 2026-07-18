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
#include "DEngine/Asset/Serializer/MaterialSerializer.h"
#include "DEngine/Asset/Serializer/ModelSerializer.h"
#include "DEngine/Scene/Serialization/SceneSerializer.h"

#include "fstream"
#include "yaml-cpp/yaml.h"

const std::string ASSET_REGISTRY_NAME = ".dassetslist";

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

	EditorAssetManager::~EditorAssetManager()
	{
		SerializeAssetRegistry();
	}

	void EditorAssetManager::Init()
	{
		DeserializeAssetRegistry();

		AddAssetsInDirectory(Project::GetAssetRegistryPath(), true);
		AddAssetsInDirectory(Project::GetResourcesRegistryPath(), false);

		SerializeAssetRegistry();
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
		else if (extension == DMODEl_FILE_EXT || extension == ".obj" || extension == ".fbx" || extension == ".gltf" || 
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
		else if (extension == DMAT_FILE_EXT)
		{
			type = AssetType::Material;
		}
		else if (extension == DMESH_FILE_EXT)
		{
			type = AssetType::Mesh;
		}
		else if (extension == DSCENE_FILE_EXT)
		{
			type = AssetType::Scene;
		}
		else
		{
			D_CORE_ERROR("EditorAssetManager::CreateAsset: unsupported file type: {0}", extension);
			type = AssetType::None;
		}

		// Проверяем, существует ли уже такой путь в реестре
		AssetHandle handle = CheckAssetRegistryForPath(path);
		if (handle != AssetHandle::Invalid())
		{
			return handle;
		}

		//TODO: дописать для остальных типов ресурсов, когда они будут готовы
		handle = CreateAsset({ type, path });

		return handle;
	}

	const AssetHandle& EditorAssetManager::CreateAsset(AssetMetadata metadata)
	{
		if (metadata.Type == AssetType::None) return AssetHandle::Invalid();

		// Проверяем, существует ли уже такой путь в реестре
		AssetHandle handle = CheckAssetRegistryForPath(metadata.FilePath);
		if (handle != AssetHandle::Invalid())
		{
			return handle;
		}

		handle = AssetHandle();
		m_AssetRegistry[handle] = metadata;

		// Добавляем файл в watcher если путь указан
		//TODO: вместо точечной обработкой типов файлов без HotReload стоит поместить это в какой-то обработчик скорее
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
		AssetHandle handle = CheckAssetRegistryForPath(path);
		if (handle != AssetHandle::Invalid())
		{
			return handle;
		}
		handle = AssetHandle();
		Ref<Mesh> mesh = nullptr;

		MeshData meshData;

		meshData.verts = verts;
		meshData.vertSize = vertsSize;
		meshData.inds = inds;
		meshData.indsSize = indsSize;

		mesh = CreateRef<Mesh>(layout, meshData);

		MeshSerializer::Serialize(mesh, meshData, path);

		m_AssetRegistry[handle] = { AssetType::Mesh, path };

		return handle;
	}

	const AssetHandle& EditorAssetManager::CreateMaterialAsset(const Ref<Material>& material, const std::filesystem::path& path)
	{
		AssetHandle handle = CheckAssetRegistryForPath(path);
		if (handle != AssetHandle::Invalid())
		{
			return handle;
		}
		handle = AssetHandle();
		MaterialSerializer::Serialize(material, path);

		m_AssetRegistry[handle] = { AssetType::Material, path };

		return handle;
	}

	const AssetHandle& EditorAssetManager::CreateModelAsset(const Ref<Model>& model, const std::filesystem::path& path)
	{
		AssetHandle handle = CheckAssetRegistryForPath(path);
		if (handle != AssetHandle::Invalid())
		{
			return handle;
		}
		handle = AssetHandle();
		ModelSerializer::Serialize(model, path);

		m_AssetRegistry[handle] = { AssetType::Model, path };

		return handle;
	}

	const AssetHandle& EditorAssetManager::CreateSceneAsset(const Ref<Scene>& scene, const std::filesystem::path& path)
	{
		AssetHandle handle = CheckAssetRegistryForPath(path);
		if (handle != AssetHandle::Invalid())
		{
			return handle;
		}
		handle = AssetHandle();
		SceneSerializer::Serialize(scene, path);

		m_AssetRegistry[handle] = { AssetType::Scene, path };

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
				D_CORE_ERROR("EditorAssetManager::GetAsset: couldn't load {0} by path {1}", AssetTypeToString(metadata.Type), metadata.FilePath.string());
				D_CORE_ASSERT(false, "");
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
		//Проверяем нет ли уже кэшированного примитива
		auto it = m_MeshPrimitives.find(type);
		if (it != m_MeshPrimitives.end())
			return it->second;

		// Проверяем, существует ли путь примитива в реестре
		std::filesystem::path meshPrimitivePath = MeshGenerator::ConstructPrimitivePath(type);
		AssetHandle handle = CheckAssetRegistryForPath(meshPrimitivePath);
		if (handle != AssetHandle::Invalid())
		{
			m_MeshPrimitives[type] = handle;
			return handle;
		}

		handle = CreateMeshPrimitive(type);
		return handle;
	}

	void EditorAssetManager::SerializeAssetRegistry()
	{
		auto path = GetAssetRegistryFilePath();

		std::ofstream file(path, std::ios::binary);
		if (!file.is_open())
		{
			D_CORE_ERROR("EditorAssetManager::SerializeAssetRegistry: Could not open file {0}", path.string());
			return;
		}

		// Записываем количество ассетов
		uint32_t count = static_cast<uint32_t>(m_AssetRegistry.size());
		file.write(reinterpret_cast<const char*>(&count), sizeof(uint32_t));

		for (const auto& [handle, metadata] : m_AssetRegistry)
		{
			// Записываем handle
			file.write(reinterpret_cast<const char*>(&handle), sizeof(AssetHandle));

			// Записываем длину строки пути и сам путь
			std::string pathStr = metadata.FilePath.string();
			uint32_t pathLen = static_cast<uint32_t>(pathStr.length());
			file.write(reinterpret_cast<const char*>(&pathLen), sizeof(uint32_t));
			file.write(pathStr.c_str(), pathLen);

			// Записываем тип ассета
			uint32_t type = static_cast<uint32_t>(metadata.Type);
			file.write(reinterpret_cast<const char*>(&type), sizeof(uint32_t));
		}

		file.close();
	}

	bool EditorAssetManager::DeserializeAssetRegistry()
	{
		auto path = GetAssetRegistryFilePath();

		if (!std::filesystem::exists(path))
		{
			D_CORE_WARN("EditorAssetManager::DeserializeAssetRegistry: File {0} doesn't exist", path.string());
			return false;
		}

		std::ifstream file(path, std::ios::binary);
		if (!file.is_open())
		{
			D_CORE_ERROR("EditorAssetManager::DeserializeAssetRegistry: Could not open file {0}", path.string());
			return false;
		}

		// Читаем количество ассетов
		uint32_t count = 0;
		file.read(reinterpret_cast<char*>(&count), sizeof(uint32_t));

		for (uint32_t i = 0; i < count; i++)
		{
			AssetHandle handle;
			file.read(reinterpret_cast<char*>(&handle), sizeof(AssetHandle));

			// Читаем длину строки пути
			uint32_t pathLen = 0;
			file.read(reinterpret_cast<char*>(&pathLen), sizeof(uint32_t));

			// Читаем сам путь
			std::string pathStr(pathLen, '\0');
			file.read(pathStr.data(), pathLen);

			// Читаем тип ассета
			uint32_t type = 0;
			file.read(reinterpret_cast<char*>(&type), sizeof(uint32_t));

			auto& metadata = m_AssetRegistry[handle];
			metadata.FilePath = pathStr;
			metadata.Type = static_cast<AssetType>(type);

			D_CORE_INFO("Deserialize asset {0} ({2}) by path {1}", AssetTypeToString(metadata.Type), metadata.FilePath.string(), std::to_string(handle));

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

		file.close();
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
		mesh = MeshGenerator::CreatePrimitive(type);

		m_AssetRegistry[handle] = { AssetType::Mesh, meshPrimitivePath };
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

	std::filesystem::path EditorAssetManager::GetAssetRegistryFilePath()
	{
		auto path = Project::GetAssetRegistryPath() / ASSET_REGISTRY_NAME;
		return path;
	}

	void EditorAssetManager::AddAssetsInDirectory(const std::filesystem::path& path, bool isVisibleInEditor)
	{
		if (!std::filesystem::exists(path))
		{
			D_CORE_WARN("EditorAssetManager::AddAssetsInDirectory: Directory {0} doesn't exist", path.string());
			return;
		}

		if (!std::filesystem::is_directory(path))
		{
			D_CORE_WARN("EditorAssetManager::AddAssetsInDirectory: {0} is not a directory", path.string());
			return;
		}

		for (const auto& entry : std::filesystem::recursive_directory_iterator(path))
		{
			if (entry.is_regular_file())
			{
				const auto& filePath = entry.path();
				
				// TODO: добавить систему обработку ислключений по типам файлов
				if (filePath.filename() == ASSET_REGISTRY_NAME)
					continue;

				// Проверяем, не добавлен ли уже этот файл
				auto it = m_PathToHandle.find(filePath);
				if (it != m_PathToHandle.end())
					continue;

				// Создаем ассет
				CreateAsset(filePath);
			}
		}
	}

	AssetHandle EditorAssetManager::CheckAssetRegistryForPath(const std::filesystem::path& path)
	{
		if (path.empty()) return AssetHandle::Invalid();

		auto it = m_PathToHandle.find(path);
		if (it != m_PathToHandle.end())
		{
			return it->second;
		}

		return AssetHandle::Invalid();
	}

}