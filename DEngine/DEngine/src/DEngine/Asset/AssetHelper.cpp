#include "dpch.h"
#include "AssetHelper.h"

#include "DEngine/Asset/AssetManager.h"
#include "DEngine/Asset/EditorAssetManager.h"
#include "DEngine/Project/Project.h"

#include <cctype>

namespace DEngine
{
	namespace
	{
		std::string LowerExtension(const std::filesystem::path& path)
		{
			std::string extension = path.extension().string();
			for (char& character : extension)
				character = static_cast<char>(std::tolower(static_cast<unsigned char>(character)));
			return extension;
		}

		bool IsExtension(const std::string& extension, std::initializer_list<const char*> candidates)
		{
			for (const char* candidate : candidates)
			{
				if (extension == candidate)
					return true;
			}
			return false;
		}
	}

	AssetType AssetHelper::TypeFromExtension(const std::filesystem::path& path)
	{
		const std::string extension = LowerExtension(path);
		if (extension.empty())
			return AssetType::None;

		if (IsExtension(extension, { ".png", ".jpg", ".jpeg", ".bmp", ".tga", ".psd" }))
			return AssetType::Texture2D;

		if (IsExtension(extension, { DMODEl_FILE_EXT, ".obj", ".fbx", ".gltf", ".glb", ".3ds", ".dae" }))
			return AssetType::Model;

		if (IsExtension(extension, { ".glsl", ".vert", ".frag", ".comp", ".geom", ".tesc", ".tese" }))
			return AssetType::Shader;

		if (extension == DMAT_FILE_EXT)
			return AssetType::Material;

		if (extension == DMESH_FILE_EXT)
			return AssetType::Mesh;

		if (extension == DSCENE_FILE_EXT)
			return AssetType::Scene;

		return AssetType::None;
	}

	std::string_view AssetHelper::NativeExtension(AssetType type)
	{
		switch (type)
		{
			case AssetType::Mesh: return DMESH_FILE_EXT;
			case AssetType::Material: return DMAT_FILE_EXT;
			case AssetType::Model: return DMODEl_FILE_EXT;
			case AssetType::Scene: return DSCENE_FILE_EXT;
			case AssetType::Shader: return ".glsl";
			default: return {};
		}
	}

	std::string_view AssetHelper::ToString(AssetType type)
	{
		switch (type)
		{
			case AssetType::None: return "AssetType::None";
			case AssetType::Scene: return "AssetType::Scene";
			case AssetType::Texture2D: return "AssetType::Texture2D";
			case AssetType::Mesh: return "AssetType::Mesh";
			case AssetType::Material: return "AssetType::Material";
			case AssetType::Model: return "AssetType::Model";
			case AssetType::Shader: return "AssetType::Shader";
		}

		return "AssetType::Invalid";
	}

	AssetType AssetHelper::FromString(std::string_view str)
	{
		if (str == "AssetType::None") return AssetType::None;
		if (str == "AssetType::Scene") return AssetType::Scene;
		if (str == "AssetType::Texture2D") return AssetType::Texture2D;
		if (str == "AssetType::Mesh") return AssetType::Mesh;
		if (str == "AssetType::Material") return AssetType::Material;
		if (str == "AssetType::Model") return AssetType::Model;
		if (str == "AssetType::Shader") return AssetType::Shader;

		return AssetType::None;
	}

	std::filesystem::path AssetHelper::NormalizePath(const std::filesystem::path& path)
	{
		if (path.empty())
			return {};

		return std::filesystem::path(path.lexically_normal().generic_string());
	}

	bool AssetHelper::SamePath(const std::filesystem::path& a, const std::filesystem::path& b)
	{
		return NormalizePath(a) == NormalizePath(b);
	}

	bool AssetHelper::IsResourcesPath(const std::filesystem::path& path)
	{
		const std::string generic = NormalizePath(path).generic_string();
		const std::string resources = NormalizePath(Project::GetResourcesRegistryPath()).generic_string();
		return generic == resources || generic.rfind(resources + "/", 0) == 0;
	}

	void AssetHelper::EnsureParentDirectory(const std::filesystem::path& path)
	{
		const std::filesystem::path parent = path.parent_path();
		if (!parent.empty() && !std::filesystem::exists(parent))
			std::filesystem::create_directories(parent);
	}

	std::string AssetHelper::DisplayName(AssetHandle handle)
	{
		if (static_cast<uint64_t>(handle) == 0)
			return "None";

		auto editor = AssetManager::Get<EditorAssetManager>();
		if (!editor || !editor->IsAssetHandleValid(handle))
			return "Missing";

		const std::filesystem::path path = editor->GetMetadata(handle).FilePath;
		const std::string stem = path.stem().string();
		if (stem == "CubePrimitive")
			return "Cube";
		if (stem == "SpherePrimitive")
			return "Sphere";

		return path.filename().string();
	}
}
