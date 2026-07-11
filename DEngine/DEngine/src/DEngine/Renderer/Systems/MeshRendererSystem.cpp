#include "dpch.h"
#include "MeshRendererSystem.h"

#include "DEngine/Scene/Components.h"
#include "DEngine/Renderer/Renderer.h"

#include "DEngine/Asset/AssetManager.h"

namespace DEngine
{
	void MeshRendererSystem::OnRender(const Timestep& ts, const Scene* scene)
	{
		auto lightComps = scene->View<DirectLightComponent, TransformComponent>();
		auto comps = scene->View<MeshRendererComponent, TransformComponent>();

		for (auto [entity, mesh, trans] : comps.each())
		{
			auto mat = AssetManager::GetAsset<Material>(mesh.materialHandle);
			auto meshObj = AssetManager::GetAsset<Mesh>(mesh.meshHandle);

			for (auto [entity, light, lightTrans] : lightComps.each()) // TODO: в будущем надо обрабатывать разные light, но пока есть только DirectLight
			{
				mat->SetFloat3("u_LightDirection", lightTrans.GetForward());
				mat->SetFloat3("u_LightColor", light.color);
			}

			Renderer::Submit(meshObj, mat, trans.trans);
		}
	}
}