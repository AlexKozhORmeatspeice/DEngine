#pragma once

#include "DEngine/Core.h"
#include "DEngine/Renderer/Mesh/Mesh.h"
#include "DEngine/Renderer/Material/Material.h"

namespace DEngine
{
	struct MeshRendererComponent
	{
		Ref<Mesh> mesh;
		Ref<Material> material;

		MeshRendererComponent() = default;
		MeshRendererComponent(const MeshRendererComponent&) = default;
		MeshRendererComponent(const Ref<Mesh>& _mesh, const Ref<Material>& _mat)
			: mesh(_mesh), material(_mat) { }
	};
}
