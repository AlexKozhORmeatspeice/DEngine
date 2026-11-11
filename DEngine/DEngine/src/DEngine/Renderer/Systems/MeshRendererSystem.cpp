#include "dpch.h"
#include "MeshRendererSystem.h"

#include "DEngine/Scene/Components.h"
#include "DEngine/Scene/Entity.h"
#include "DEngine/Renderer/Renderer.h"

#include "DEngine/Asset/AssetManager.h"
#include "DEngine/Profiler/Profiler.h"

#include "DEngine/Jobs/JobSystem.h"


namespace DEngine
{
	struct RenderObj
	{
		bool isRendered = false;
		AssetHandle materialHandle{ uint64_t{ 0 } };
		AssetHandle meshHandle{ uint64_t{ 0 } };
		Ref<Material> mat;
		Ref<Mesh> mesh;
		glm::mat4 model{ 1.0f };
	};

	void MeshRendererSystem::OnRender(const Timestep& ts, Scene* scene)
	{
		D_PROFILE_FUNCTION();
		auto lightComps = scene->View<DirectLightComponent, TransformComponent>();

		auto comps = scene->View<TagComponent, MeshRendererComponent, TransformComponent>();
		const auto* entityStorage = comps.handle();

		struct LightData
		{
			glm::vec3 lookDir;
			glm::vec3 color;
		};

		std::vector<LightData> lights;

		for (auto [entity, light, lightTrans] : lightComps.each()) // TODO: в будущем надо обрабатывать разные light, но пока есть только DirectLight
		{
			if (!scene->IsActive(Entity(entity, scene)))
				continue;

			lights.push_back({ lightTrans.GetForward(), light.color });
		}

		const uint32_t count = entityStorage ? static_cast<uint32_t>(entityStorage->size()) : 0;
		if (count == 0) return;

		std::vector<RenderObj> renderObjs(count);

		Job prepare(count, [&](const JobRange& range, uint32_t)
		{
			for (uint32_t i = range.Start; i < range.End; ++i)
			{
				const auto entity = (*entityStorage)[i];
				if (!comps.contains(entity))
					continue;

				const auto& tag = comps.get<TagComponent>(entity);
				const auto& mesh = comps.get<MeshRendererComponent>(entity);
				const auto& trans = comps.get<TransformComponent>(entity);

				if (!tag.activeSelf || trans.unactiveByParent)
					continue;

				renderObjs[i].isRendered = true;
				renderObjs[i].materialHandle = mesh.materialHandle;
				renderObjs[i].meshHandle = mesh.meshHandle;
				renderObjs[i].mat = AssetManager::GetAsset<Material>(mesh.materialHandle);
				renderObjs[i].mesh = AssetManager::GetAsset<Mesh>(mesh.meshHandle);
				renderObjs[i].model = trans.GetModelMatrix();

				//TODO: в будущем надо перенести логику отрисовки тоже в параллельный блок, но для этого надо сильно менять архитектуру рендера и использовать RenderCommand
			}
		});

		JobSystem::Schedule(prepare);
		JobSystem::Wait(prepare);
		
		{
			D_PROFILE_SCOPE("Renderer upload")
			for (RenderObj& obj : renderObjs)
			{
				if (!obj.isRendered)
					continue;

				if (!obj.mat)
				{
					obj.mat = AssetManager::GetAsset<Material>(obj.materialHandle);
				}
				if (!obj.mesh)
				{
					obj.mesh = AssetManager::GetAsset<Mesh>(obj.meshHandle);
				}

				if (!obj.mat || !obj.mesh)
					continue;

				if (!lights.empty())
				{
					obj.mat->SetFloat3("u_LightDirection", lights[0].lookDir);
					obj.mat->SetFloat3("u_LightColor", lights[0].color);
				}

				Renderer::Submit(obj.mesh, obj.mat, obj.model);
			}
		}
	}
}