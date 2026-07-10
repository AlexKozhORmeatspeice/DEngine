#pragma once
#include "DEngine/Core.h"

#include "unordered_map"
#include "vector"

#include "DEngine/Asset/Asset.h"

namespace DEngine
{
	//Пара мэша и материала
	struct MeshRenderData : public std::pair<AssetHandle, AssetHandle>
	{
		AssetHandle MeshHandle = first;
		AssetHandle MaterialHandle = second;

		MeshRenderData() = default;
		MeshRenderData(AssetHandle mesh, AssetHandle material)
			: std::pair<AssetHandle, AssetHandle>(mesh, material) {}
	};

	class Model : public Asset
	{
	public:
		Model(const std::vector<MeshRenderData>& renderData, const std::string& name = "");

		static AssetType GetStaticType() { return AssetType::Model; }
		virtual AssetType GetType() const override { return GetStaticType(); }

		const std::vector<MeshRenderData>& GetRenderData() const { return m_RenderDatas; }

	private:
		std::vector<MeshRenderData> m_RenderDatas;

		std::string m_Name;
	};
}
