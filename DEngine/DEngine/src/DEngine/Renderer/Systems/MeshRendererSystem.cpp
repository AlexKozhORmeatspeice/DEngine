#include "dpch.h"
#include "MeshRendererSystem.h"

#include "DEngine/Scene/Components.h"
#include "DEngine/Renderer/Renderer.h"

#include "DEngine/Asset/AssetManager.h"

namespace DEngine
{
	void MeshRendererSystem::OnRender(const Timestep& ts, const Scene* scene)
	{
		auto comps = scene->View<MeshRendererComponent, TransformComponent>();

		for (auto [entity, mesh, trans] : comps.each())
		{
			auto mat = AssetManager::GetAsset<Material>(mesh.materialHandle);
			auto meshObj = AssetManager::GetAsset<Mesh>(mesh.meshHandle);

			Renderer::Submit(meshObj, mat, trans.trans);
		}
	}
}