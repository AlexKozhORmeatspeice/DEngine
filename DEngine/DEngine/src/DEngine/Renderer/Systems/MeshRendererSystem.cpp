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

		struct LightData
		{
			glm::vec3 lookDir;
			glm::vec3 color;
		};

		std::vector<LightData> lights;

		for (auto [entity, light, lightTrans] : lightComps.each()) // TODO: в будущем надо обрабатывать разные light, но пока есть только DirectLight
		{
			lights.push_back({ lightTrans.GetForward(), light.color });
		}

		for (auto [entity, mesh, trans] : comps.each())
		{
			auto mat = AssetManager::GetAsset<Material>(mesh.materialHandle);
			auto meshObj = AssetManager::GetAsset<Mesh>(mesh.meshHandle);

			if (lights.size() > 0)
			{
				mat->SetFloat3("u_LightDirection", lights[0].lookDir);
				mat->SetFloat3("u_LightColor", lights[0].color);
			}

			Renderer::Submit(meshObj, mat, trans.trans);
		}
	}
}