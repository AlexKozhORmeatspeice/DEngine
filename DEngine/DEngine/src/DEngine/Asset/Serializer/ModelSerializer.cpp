// ModelSerializer.cpp
#include "dpch.h"
#include "ModelSerializer.h"
#include <fstream>

namespace DEngine
{
	void ModelSerializer::Serialize(const Ref<Model>& model, const std::filesystem::path& path)
	{
		std::ofstream file(path, std::ios::binary);
		if (!file.is_open())
			return;

		const auto& renderData = model->GetRenderData();

		// Записываем количество MeshRenderData
		uint32_t count = static_cast<uint32_t>(renderData.size());
		file.write(reinterpret_cast<const char*>(&count), sizeof(uint32_t));

		// Записываем каждую пару MeshHandle и MaterialHandle
		for (const auto& data : renderData)
		{
			file.write(reinterpret_cast<const char*>(&data.MeshHandle), sizeof(AssetHandle));
			file.write(reinterpret_cast<const char*>(&data.MaterialHandle), sizeof(AssetHandle));
		}

		file.close();
	}

	DeserializeModelResult ModelSerializer::Deserialize(const std::filesystem::path& path)
	{
		DeserializeModelResult result;
		result.isSuccessful = false;
		result.model = nullptr;

		std::ifstream file(path, std::ios::binary);
		if (!file.is_open())
			return result;

		// Читаем количество MeshRenderData
		uint32_t count = 0;
		file.read(reinterpret_cast<char*>(&count), sizeof(uint32_t));

		std::vector<MeshRenderData> renderData;
		renderData.reserve(count);

		// Читаем каждую пару
		for (uint32_t i = 0; i < count; i++)
		{
			AssetHandle meshHandle;
			AssetHandle materialHandle;

			file.read(reinterpret_cast<char*>(&meshHandle), sizeof(AssetHandle));
			file.read(reinterpret_cast<char*>(&materialHandle), sizeof(AssetHandle));

			renderData.emplace_back(meshHandle, materialHandle);
		}

		file.close();

		result.model = CreateRef<Model>(renderData);
		result.isSuccessful = true;

		return result;
	}
}