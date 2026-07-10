#pragma once

#include "DEngine/Core.h"
#include "DEngine/Renderer/Mesh/Mesh.h"
#include "DEngine/Renderer/Material/Material.h"

#include "DEngine/Asset/Asset.h"

namespace DEngine
{
	struct MeshRendererComponent
	{
		AssetHandle meshHandle;
		AssetHandle materialHandle;

		MeshRendererComponent() = default;
		MeshRendererComponent(const MeshRendererComponent&) = default;
		MeshRendererComponent(const AssetHandle& _mesh, const AssetHandle& _mat)
			: meshHandle(_mesh), materialHandle(_mat) { }
	};
}
