#include "dpch.h"
#include "EditorAssetManager.h"
#include "Importer/AssetImporter.h"
#include "Importer/TextureImporter.h"
#include "Importer/MeshImporter.h"

#include "DEngine/Core.h"

#include "DEngine/Project/Project.h"
#include "DEngine/Asset/FileWatcher.h"

#include "DEngine/Renderer/Shader/Shader.h"
#include "DEngine/Renderer/Material/Material.h"
#include "DEngine/Renderer/Mesh/Mesh.h"
#include "DEngine/Renderer/Texture/Texture.h"
#include "DEngine/Asset/Asset.h"
#include "DEngine/Asset/AssetHelper.h"

#include "DEngine/Asset/Serializer/MeshSerializer.h"
#include "DEngine/Asset/Serializer/MaterialSerializer.h"
#include "DEngine/Asset/Serializer/ModelSerializer.h"
#include "DEngine/Scene/Serialization/SceneSerializer.h"

#include "DEngine/Jobs/JobSystem.h"
#include "DEngine/Profiler/Profiler.h"

#include "fstream"
#include "yaml-cpp/yaml.h"

#include <chrono>

const std::string ASSET_REGISTRY_NAME = ".dassetslist";

namespace DEngine
{
	namespace
	{
		const AssetHandle& InvalidAssetHandle();
	}

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
	}

	EditorAssetManager::~EditorAssetManager()
	{
	}

	void EditorAssetManager::Init()
	{
		D_PROFILE_FUNCTION();
		DeserializeAssetRegistry();

		AddAssetsInDirectory(Project::GetAssetRegistryPath(), true);
		AddAssetsInDirectory(Project::GetResourcesRegistryPath(), false);

		SerializeAssetRegistry();

		CreateBaseRendererShader();
		CreateEmptyTexture();
		CreateBaseRendererMaterial();

		CreateWireframeShader();
		CreateBaseRendererMaterialWireframe();

		CreateMeshPrimitive(PrimitiveType::Cube);

		SetupFileWatcher();
		SerializeAssetRegistry();
	}

	void EditorAssetManager::Update()
	{
		D_PROFILE_FUNCTION();
		if (!m_HotReloadEnabled)
			return;

		m_FileWatcher.Update();
		if (m_PendingReloads.empty())
			return;

		const AssetHandle handle = m_PendingReloads.front();
		m_PendingReloads.erase(m_PendingReloads.begin());
		ReloadAsset(handle);
	}


	void EditorAssetManager::Shutdown()
	{
		for (auto& [handle, request] : m_InFlightLoads)
		{
			if (request && request->Decode)
				JobSystem::Wait(*request->Decode);
		}

		UploadPendingAssets(true);
		SerializeAssetRegistry();
	}

	void EditorAssetManager::ReloadAsset(const AssetHandle& handle)
	{
		D_PROFILE_FUNCTION();
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

		if (metadata.Type == AssetType::Shader || metadata.Type == AssetType::Texture2D)
		{
			for (auto& entry : m_LoadedAssets)
			{
				Ref<Material> material = std::dynamic_pointer_cast<Material>(entry.second);
				if (!material)
					continue;

				if (metadata.Type == AssetType::Shader && material->GetShaderHandle() == handle)
					material->InvalidateCachedShader();
				else if (metadata.Type == AssetType::Texture2D)
					material->InvalidateCachedTexture(handle);
			}
		}

		if (!metadata.FilePath.empty() && std::filesystem::exists(metadata.FilePath))
		{
			const auto writeTime = std::filesystem::last_write_time(metadata.FilePath);
			m_AssetLastLoadTime[handle] = writeTime;
			m_FileWatcher.SetLastWriteTime(metadata.FilePath, writeTime);
		}

		D_CORE_INFO("Hot reload: {0} reloaded successfully", AssetHelper::ToString(metadata.Type));
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
		D_PROFILE_FUNCTION();
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
		D_PROFILE_FUNCTION();
		const std::filesystem::path normalized = AssetHelper::NormalizePath(path);
		if (const AssetHandle* existing = FindHandleByPath(normalized))
			return *existing;

		const AssetType type = AssetHelper::TypeFromExtension(normalized);
		if (type == AssetType::None)
		{
			D_CORE_ERROR("EditorAssetManager::CreateAsset: unsupported file type: {0}", normalized.extension().string());
			return InvalidAssetHandle();
		}

		return EmplaceAsset(AssetHandle(), { type, normalized }, nullptr);
	}

	const AssetHandle& EditorAssetManager::CreateAsset(AssetMetadata metadata)
	{
		if (metadata.Type == AssetType::None)
			return InvalidAssetHandle();

		metadata.FilePath = AssetHelper::NormalizePath(metadata.FilePath);
		if (const AssetHandle* existing = FindHandleByPath(metadata.FilePath))
			return *existing;

		return EmplaceAsset(AssetHandle(), metadata, nullptr);
	}


	const AssetHandle& EditorAssetManager::CreateMeshAsset(const BufferLayout& layout,
														   float* verts, uint32_t vertsSize,
														   uint32_t* inds, uint32_t indsSize,
														   const std::filesystem::path& path)
	{
		const std::filesystem::path normalized = AssetHelper::NormalizePath(path);
		if (const AssetHandle* existing = FindHandleByPath(normalized))
			return *existing;

		MeshData meshData;
		meshData.verts = verts;
		meshData.vertSize = vertsSize;
		meshData.inds = inds;
		meshData.indsSize = indsSize;

		Ref<Mesh> mesh = CreateRef<Mesh>(layout, meshData);
		AssetHelper::EnsureParentDirectory(normalized);
		MeshSerializer::Serialize(mesh, meshData, normalized);

		const AssetHandle& handle = EmplaceAsset(AssetHandle(), { AssetType::Mesh, normalized }, mesh);
		SerializeAssetRegistry();
		return handle;
	}

	const AssetHandle& EditorAssetManager::CreateMaterialAsset(const Ref<Material>& material, const std::filesystem::path& path)
	{
		const std::filesystem::path normalized = AssetHelper::NormalizePath(path);
		AssetHelper::EnsureParentDirectory(normalized);

		if (const AssetHandle* existing = FindHandleByPath(normalized))
		{
			if (material)
			{
				MaterialSerializer::Serialize(material, normalized);
				material->handle = *existing;
				m_LoadedAssets[*existing] = material;
			}
			SerializeAssetRegistry();
			return *existing;
		}

		if (material)
			MaterialSerializer::Serialize(material, normalized);

		const AssetHandle& handle = EmplaceAsset(AssetHandle(), { AssetType::Material, normalized }, material);
		SerializeAssetRegistry();
		return handle;
	}

	const AssetHandle& EditorAssetManager::CreateModelAsset(const Ref<Model>& model, const std::filesystem::path& path)
	{
		const std::filesystem::path normalized = AssetHelper::NormalizePath(path);
		AssetHelper::EnsureParentDirectory(normalized);

		if (const AssetHandle* existing = FindHandleByPath(normalized))
		{
			if (model)
			{
				ModelSerializer::Serialize(model, normalized);
				model->handle = *existing;
				m_LoadedAssets[*existing] = model;
			}
			SerializeAssetRegistry();
			return *existing;
		}

		if (model)
			ModelSerializer::Serialize(model, normalized);

		const AssetHandle& handle = EmplaceAsset(AssetHandle(), { AssetType::Model, normalized }, model);
		SerializeAssetRegistry();
		return handle;
	}

	const AssetHandle& EditorAssetManager::CreateSceneAsset(const Ref<Scene>& scene, const std::filesystem::path& path)
	{
		const std::filesystem::path normalized = AssetHelper::NormalizePath(path);
		AssetHelper::EnsureParentDirectory(normalized);

		// Init registers the scene path before the file exists. Reuse that handle
		// and write the scene into it, otherwise the editor and the registry disagree.
		if (const AssetHandle* existing = FindHandleByPath(normalized))
		{
			if (scene)
			{
				SceneSerializer::Serialize(scene, normalized);
				scene->handle = *existing;
				m_LoadedAssets[*existing] = scene;
			}
			SerializeAssetRegistry();
			return *existing;
		}

		if (scene)
			SceneSerializer::Serialize(scene, normalized);

		const AssetHandle& handle = EmplaceAsset(AssetHandle(), { AssetType::Scene, normalized }, scene);
		SerializeAssetRegistry();
		return handle;
	}

	Ref<Asset> EditorAssetManager::WaitForInFlight(const std::shared_ptr<AssetLoadRequest>& load)
	{
		const bool onGraphicsThread = JobSystem::IsInitialized() && JobSystem::ThreadIndex() == 0;
		std::unique_lock<std::mutex> lock(load->Mutex);
		while (!load->Done)
		{
			if (!onGraphicsThread)
			{
				load->Ready.wait(lock, [&]() { return load->Done; });
				break;
			}

			lock.unlock();
			JobSystem::RunPinnedTasks();
			lock.lock();
			if (!load->Done)
				load->Ready.wait_for(lock, std::chrono::milliseconds(1));
		}
		return load->Result;
	}

	Ref<Asset> EditorAssetManager::GetPlaceholder(const AssetMetadata& metadata)
	{
		switch (metadata.Type)
		{
			case AssetType::Texture2D:
			{
				const auto placeholder = m_LoadedAssets.find(m_EmptyTextureHandle);
				if (placeholder != m_LoadedAssets.end())
					return placeholder->second;

				break;
			}
			case AssetType::Material:
			{
				const auto placeholder = m_LoadedAssets.find(m_BaseMaterial);
				if (placeholder != m_LoadedAssets.end())
					return placeholder->second;

				break;
			}
			case AssetType::Mesh:
			{
				const auto placeholder = m_LoadedAssets.find(GetPrimitiveMeshHandle(m_PlaceholderPrimitive));
				if (placeholder != m_LoadedAssets.end())
					return placeholder->second;
				break;
			}
		default:
			break;
		}

		return nullptr;
	}

	void EditorAssetManager::QueueDecodedAsset(AssetHandle handle, const AssetMetadata& metadata)
	{
		D_PROFILE_FUNCTION();

		PendingUpload pending;

		switch (metadata.Type)
		{
			case AssetType::Texture2D:
			{
				auto texturePending = PendingTextureUpload();
				texturePending.Handle = handle;

				DecodedTexture decoded = TextureImporter::DecodeTexture2D(metadata.FilePath);
				if (!decoded.Succeeded)
				{
					texturePending.Failed = true;
				}
				else
				{
					texturePending.Spec = decoded.Spec;
					texturePending.Pixels.Data = decoded.Pixels.Data;
					texturePending.Pixels.Size = decoded.Pixels.Size;
					decoded.Pixels.Data = nullptr;
					decoded.Pixels.Size = 0;
				}
				pending = texturePending;
			}
			break;
			case AssetType::Mesh:
			{
				auto meshPending = PendingMeshUpload();
				meshPending.Handle = handle;

				DecodedMesh decoded = MeshImporter::DecodeMesh(metadata.FilePath);
				if (!decoded.Succeeded)
				{
					meshPending.Failed = true;
				}
				else
				{
					meshPending.Layout = std::move(decoded.Layout);
					meshPending.Data = decoded.Data;
					decoded.Data.verts = nullptr;
					decoded.Data.inds = nullptr;
				}

				pending = meshPending;
			}
			break;
		default: //Если ресурс нельзя поставить в очередь - выходим
			return;
		}

		std::lock_guard<std::mutex> lock(m_PendingMutex);
		m_PendingAssets.push_back(std::move(pending));
	}

	void EditorAssetManager::UploadPendingAssets(bool flushAll)
	{
		D_PROFILE_FUNCTION();
		constexpr uint64_t kUploadBudget = 5;

		std::vector<PendingUpload> batch;
		//Определяем бюджет загрузки на кадр
		{
			std::lock_guard<std::mutex> lock(m_PendingMutex);
			uint64_t bytes = 0;
			while (!m_PendingAssets.empty())
			{
				uint64_t size = 0;
				auto asset = m_PendingAssets.front();
				std::visit(overloaded
				{
					[&](PendingTextureUpload& t)
					{
						//size += t.Pixels.Size;
						size += 1;
					},
					[&](PendingMeshUpload& m)
					{
						//size += m.Data.vertSize;
						//size += static_cast<uint64_t>(m.Data.indsSize) * sizeof(uint32_t);
						size += 1;
					},
				}, asset);

				if (!flushAll && !batch.empty() && bytes + size > kUploadBudget)
					break;

				bytes += size;
				batch.push_back(m_PendingAssets.front());
				m_PendingAssets.erase(m_PendingAssets.begin());
			}
		}

		//Загружаем
		for (PendingUpload& item : batch)
		{
			Ref<Asset> resAsset;
			AssetHandle handle;

			std::visit(overloaded
			{
				[&](PendingTextureUpload& t)
				{
					handle = t.Handle;
					if (!t.Failed)
					{
						resAsset = Texture2D::Create(t.Spec, t.Pixels);
						t.Pixels.Release();
					}

				},
				[&](PendingMeshUpload& m)
				{
					handle = m.Handle;
					if (!m.Failed)
						resAsset = MeshImporter::UploadMesh(m.Layout, m.Data);
				},
			}, item);

			if (!resAsset)
			{
				const auto& metadata = GetMetadata(handle);
				D_CORE_ERROR("EditorAssetManager: couldn't load asset {0}", metadata.FilePath.string());
			}
			else
			{
				resAsset->handle = handle;
			}

			std::unique_lock<std::shared_mutex> lock(m_LoadedAssetsMutex);
			m_LoadedAssets[handle] = resAsset;
			if (resAsset)
			{
				const auto& metadata = GetMetadata(handle);
				if (!metadata.FilePath.empty() && std::filesystem::exists(metadata.FilePath))
					m_AssetLastLoadTime[handle] = std::filesystem::last_write_time(metadata.FilePath);
			}
			m_InFlightLoads.erase(handle);
		}
	}

	Ref<Asset> EditorAssetManager::GetAsset(const AssetHandle& handle)
	{
		D_PROFILE_FUNCTION();

		{
			//Проверяем не был ли уже загружен ассет
			std::shared_lock<std::shared_mutex> readLock(m_LoadedAssetsMutex);
			const auto loaded = m_LoadedAssets.find(handle);
			if (loaded != m_LoadedAssets.end())
				return loaded->second;

			//Проверяем не находится ли объект в очереди на загрузку
			const auto inflight = m_InFlightLoads.find(handle);
			if (inflight != m_InFlightLoads.end() && inflight->second->ReturnsPlaceholder)
				return GetPlaceholder(inflight->second->metadata);
		}

		std::shared_ptr<AssetLoadRequest> load;
		bool startLoad = false;
		AssetMetadata metadata;
		std::shared_ptr<AssetLoadRequest> asyncLoad;
		Ref<Asset> placeholder;

		{
			std::unique_lock<std::shared_mutex> lock(m_LoadedAssetsMutex);
			//Повторные потоки если загрузка произошла уже в другом потоке
			const auto loaded = m_LoadedAssets.find(handle);
			if (loaded != m_LoadedAssets.end())
				return loaded->second;

			const auto inflight = m_InFlightLoads.find(handle);
			if (inflight != m_InFlightLoads.end())
			{
				if (inflight->second->ReturnsPlaceholder)
					return GetPlaceholder(inflight->second->metadata);
				load = inflight->second;
			}
			else //Проверяем нужно ли загружать объект асинхронно. Если да - создаем Job для этого
			{
				metadata = GetMetadata(handle);
				const bool onGraphicsThread = !JobSystem::IsInitialized() || JobSystem::ThreadIndex() == 0;
				const bool asyncAsset =   (metadata.Type == AssetType::Texture2D || metadata.Type == AssetType::Mesh)
										  && handle != m_EmptyTextureHandle && handle != GetPrimitiveMeshHandle(m_PlaceholderPrimitive)
										  && JobSystem::IsInitialized()
										  && JobSystem::ThreadCount() > 1;

				if (metadata.Type == AssetType::None)
					return nullptr;
				if (!asyncAsset && metadata.Type != AssetType::Texture2D && metadata.Type != AssetType::Mesh && !onGraphicsThread)
					return nullptr;

				if (asyncAsset) //Создание Job на потоках кроме основного
				{
					asyncLoad = std::make_shared<AssetLoadRequest>();
					asyncLoad->metadata = metadata;
					asyncLoad->ReturnsPlaceholder = true;

					const uint32_t workers = JobSystem::ThreadCount() - 1;
					const uint32_t thread = 1 + (m_NextDecodeThread++ % workers);
					const std::filesystem::path path = metadata.FilePath;
					asyncLoad->Decode = CreateScope<PinnedJob>(thread, [this, handle, metadata]()
					{
						QueueDecodedAsset(handle, metadata);
					});
					m_InFlightLoads.emplace(handle, asyncLoad);
					placeholder = GetPlaceholder(asyncLoad->metadata);
				}
				else
				{
					load = std::make_shared<AssetLoadRequest>();
					m_InFlightLoads.emplace(handle, load);
					startLoad = true;
				}
			}
		}

		if (asyncLoad) // Инитим Job для асинхронной загрузки
		{
			JobSystem::Schedule(*asyncLoad->Decode);
			return placeholder;
		}

		if (!startLoad) //Даем простояться основному потоку для загрузки
			return WaitForInFlight(load);

		Ref<Asset> asset;
		{
			D_PROFILE_SCOPE("Load new asset");
			asset = AsssetImporter::ImportAsset(handle, metadata);
		}

		if (!asset)
		{
			D_CORE_ERROR("EditorAssetManager::GetAsset: couldn't load {0} by path {1}", AssetHelper::ToString(metadata.Type), metadata.FilePath.string());
		}

		//Проверка готов ли ресурс
		{
			std::lock_guard<std::mutex> readyLock(load->Mutex);
			load->Result = asset;
			load->Done = true;
		}
		load->Ready.notify_all();

		//Сохраняем ассет
		{
			std::unique_lock<std::shared_mutex> lock(m_LoadedAssetsMutex);
			m_LoadedAssets.emplace(handle, asset);
			if (asset && !metadata.FilePath.empty() && std::filesystem::exists(metadata.FilePath))
				m_AssetLastLoadTime[handle] = std::filesystem::last_write_time(metadata.FilePath);
			m_InFlightLoads.erase(handle);
		}

		return asset;
	}

	bool EditorAssetManager::IsAssetHandleValid(const AssetHandle& handle) const
	{
		if (handle == 0) return false;

		return m_AssetRegistry.find(handle) != m_AssetRegistry.end();
	}

	bool EditorAssetManager::IsAssetLoaded(const AssetHandle& handle) const
	{
		std::shared_lock<std::shared_mutex> readLock(m_LoadedAssetsMutex);
		return m_LoadedAssets.find(handle) != m_LoadedAssets.end();
	}

	AssetHandle EditorAssetManager::GetHandleByPath(const std::filesystem::path& path)
	{
		if (const AssetHandle* found = FindHandleByPath(path))
			return *found;

		return AssetHandle(static_cast<uint64_t>(AssetHandle::Invalid()));
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

	const AssetHandle& EditorAssetManager::GetPrimitiveMeshHandle(PrimitiveType type)
	{
		auto it = m_MeshPrimitives.find(type);
		if (it != m_MeshPrimitives.end())
		{
			auto reg = m_AssetRegistry.find(it->second);
			if (reg != m_AssetRegistry.end())
				return reg->first;
		}

		const std::filesystem::path meshPrimitivePath = AssetHelper::NormalizePath(MeshGenerator::ConstructPrimitivePath(type));
		if (const AssetHandle* existing = FindHandleByPath(meshPrimitivePath))
		{
			m_MeshPrimitives[type] = *existing;
			return *existing;
		}

		return CreateMeshPrimitive(type);
	}

	void EditorAssetManager::SerializeAssetRegistry()
	{
		D_PROFILE_FUNCTION();
		auto path = GetAssetRegistryFilePath();
		AssetHelper::EnsureParentDirectory(path);

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
		D_PROFILE_FUNCTION();
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
			metadata.FilePath = AssetHelper::NormalizePath(pathStr);
			metadata.Type = static_cast<AssetType>(type);

			D_CORE_INFO("Deserialize asset {0} ({2}) by path {1}", AssetHelper::ToString(metadata.Type), metadata.FilePath.string(), std::to_string(handle));

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

	void EditorAssetManager::CreateWireframeShader()
	{
		m_WireframeShaderHandle = CreateAsset({ AssetType::Shader, "assets/shaders/wireframe.glsl" });
	}

	void EditorAssetManager::CreateBaseRendererMaterialWireframe()
	{
		Ref<Material> mat = CreateRef<Material>(m_WireframeShaderHandle);
		std::string filename = std::string("assets/materials/wireframe") + std::string(AssetHelper::NativeExtension(AssetType::Material));
		m_BaseMaterialWireframe = CreateMaterialAsset(mat, filename);
	}

	void EditorAssetManager::CreateBaseRendererMaterial()
	{
		Ref<Material> mat = CreateRef<Material>(m_BaseShaderHandle);
		std::string filename = std::string("assets/materials/BaseMaterial") + std::string(AssetHelper::NativeExtension(AssetType::Material));
		m_BaseMaterial = CreateMaterialAsset(mat, filename);
	}

	void EditorAssetManager::CreateEmptyTexture()
	{
		const std::filesystem::path path = AssetHelper::NormalizePath("assets/textures/white_square.png");
		m_EmptyTextureHandle = CreateAsset({ AssetType::Texture2D, path });
		if (std::filesystem::exists(path))
		{
			if (Ref<Asset> loaded = GetAsset(m_EmptyTextureHandle))
			{
				loaded->handle = m_EmptyTextureHandle;
				return;
			}
		}

		TextureSpecification spec;
		spec.Width = 1;
		spec.Height = 1;
		spec.Format = ImageFormat::RGBA8;

		const uint8_t white[4] = { 255, 255, 255, 255 };
		DataBuffer buffer(white, sizeof(white));
		Ref<Texture2D> texture = Texture2D::Create(spec, buffer);
		if (!texture)
			return;

		texture->handle = m_EmptyTextureHandle;
		m_LoadedAssets[m_EmptyTextureHandle] = texture;
	}

	const AssetHandle& EditorAssetManager::CreateMeshPrimitive(PrimitiveType type)
	{
		const std::filesystem::path meshPrimitivePath = AssetHelper::NormalizePath(MeshGenerator::ConstructPrimitivePath(type));
		if (const AssetHandle* existing = FindHandleByPath(meshPrimitivePath))
		{
			m_MeshPrimitives[type] = *existing;
			return *existing;
		}

		Ref<Mesh> mesh = MeshGenerator::CreatePrimitive(type);
		const AssetHandle& handle = EmplaceAsset(AssetHandle(), { AssetType::Mesh, meshPrimitivePath }, mesh);
		m_MeshPrimitives[type] = handle;
		SerializeAssetRegistry();
		return handle;
	}

	void EditorAssetManager::SetupFileWatcher()
	{
		D_PROFILE_FUNCTION();
		auto assetsPath = Project::GetAssetRegistryPath();
		
		if (!std::filesystem::exists(assetsPath))
		{
			D_CORE_WARN("EditorAssetManager: Assets directory {0} doesn't exist", assetsPath.string());
			return;
		}

		for (const auto& entry : m_AssetRegistry)
		{
			const AssetMetadata& metadata = entry.second;
			if (metadata.FilePath.empty() || !std::filesystem::exists(metadata.FilePath))
				continue;

			m_FileWatcher.AddWatchPath(metadata.FilePath, [this](const std::filesystem::path& changedPath) {
				OnFileChanged(changedPath);
			});
		}

		D_CORE_INFO("EditorAssetManager: FileWatcher setup complete, watching {0}", assetsPath.string());
	}

	void EditorAssetManager::OnFileChanged(const std::filesystem::path& path)
	{
		if (!m_HotReloadEnabled)
			return;

		const auto it = m_PathToHandle.find(AssetHelper::NormalizePath(path));
		if (it == m_PathToHandle.end())
			return;

		const AssetHandle handle = it->second;
		if (!IsAssetLoaded(handle))
			return;

		if (std::find(m_PendingReloads.begin(), m_PendingReloads.end(), handle) != m_PendingReloads.end())
			return;

		m_PendingReloads.push_back(handle);
	}

	std::filesystem::path EditorAssetManager::GetAssetRegistryFilePath()
	{
		auto path = Project::GetAssetRegistryPath() / ASSET_REGISTRY_NAME;
		return path;
	}

	void EditorAssetManager::AddAssetsInDirectory(const std::filesystem::path& path, bool isVisibleInEditor)
	{
		D_PROFILE_FUNCTION();
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
				if (FindHandleByPath(filePath))
					continue;

				// Создаем ассет
				CreateAsset(filePath);
			}
		}
	}

	namespace
	{
		const AssetHandle& InvalidAssetHandle()
		{
			static const AssetHandle handle(static_cast<uint64_t>(AssetHandle::Invalid()));
			return handle;
		}
	}

	const AssetHandle* EditorAssetManager::FindHandleByPath(const std::filesystem::path& path)
	{
		const std::filesystem::path normalized = AssetHelper::NormalizePath(path);
		if (normalized.empty())
			return nullptr;

		auto mapped = m_PathToHandle.find(normalized);
		if (mapped != m_PathToHandle.end())
		{
			auto reg = m_AssetRegistry.find(mapped->second);
			if (reg != m_AssetRegistry.end())
				return &reg->first;
		}

		const std::string wanted = normalized.generic_string();
		for (auto reg = m_AssetRegistry.begin(); reg != m_AssetRegistry.end(); ++reg)
		{
			if (reg->second.FilePath.generic_string() != wanted)
				continue;

			m_PathToHandle[normalized] = reg->first;
			m_HandleToPath[reg->first] = normalized;
			return &reg->first;
		}

		return nullptr;
	}

	const AssetHandle& EditorAssetManager::EmplaceAsset(AssetHandle handle, const AssetMetadata& metadata, const Ref<Asset>& loaded)
	{
		if (static_cast<uint64_t>(handle) == AssetHandle::Invalid())
			handle = AssetHandle();

		AssetMetadata storedMeta = metadata;
		storedMeta.FilePath = AssetHelper::NormalizePath(metadata.FilePath);

		m_AssetRegistry[handle] = storedMeta;
		const AssetHandle& storedHandle = m_AssetRegistry.find(handle)->first;

		if (!storedMeta.FilePath.empty())
		{
			m_PathToHandle[storedMeta.FilePath] = storedHandle;
			m_HandleToPath[storedHandle] = storedMeta.FilePath;

			if (storedMeta.Type != AssetType::Mesh && std::filesystem::exists(storedMeta.FilePath))
			{
				m_FileWatcher.AddWatchPath(storedMeta.FilePath, [this](const std::filesystem::path& changedPath) {
					OnFileChanged(changedPath);
				});
			}
		}

		if (loaded)
		{
			loaded->handle = storedHandle;
			m_LoadedAssets[storedHandle] = loaded;

			if (!storedMeta.FilePath.empty() && std::filesystem::exists(storedMeta.FilePath))
				m_AssetLastLoadTime[storedHandle] = std::filesystem::last_write_time(storedMeta.FilePath);
		}

		return storedHandle;
	}
}