#include "dpch.h"
#include "MeshRendererSystem.h"

#include "DEngine/Scene/Components.h"
#include "DEngine/Renderer/Renderer.h"

namespace DEngine
{
	void MeshRendererSystem::OnRender(const Timestep& ts, const Scene* scene)
	{
		auto comps = scene->View<MeshRendererComponent, TransformComponent>();

		for (auto [entity, mesh, trans] : comps.each())
		{
			Renderer::Submit(mesh.mesh, mesh.material, trans.trans);
		}
	}
}