#include "dpch.h"
#include "DebugRendererSystem.h"

#include "DEngine/Scene/Components.h"
#include "DEngine/Renderer/Renderer.h"

#include "DEngine/Asset/AssetManager.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"

namespace DEngine
{
	void DebugRendererSystem::OnRender(const Timestep& ts, const Scene* scene)
	{
		auto comps = scene->View<ColliderComponent, TransformComponent, DebugComponent>();

		for (auto [entity, collider, trans, debug] : comps.each())
		{
			if (!debug.debugOn) continue;

			auto mat = AssetManager::GetAsset<Material>(AssetManager::GetBaseRendererMaterialWireframeHandle());
			Ref<Mesh> meshObj = nullptr;

			glm::mat4 transform;

			DEngine::AssetHandle handle;

			switch (collider.type)
			{
			case ColliderType::Box:
				handle = AssetManager::GetPrimitiveMesh(PrimitiveType::Cube);

				transform = glm::translate(glm::mat4(1.0f), collider.offset + trans.GetPosition()) *
							glm::toMat4(trans.GetRotation()) *
							glm::scale(glm::mat4(1.0f), collider.size);

				meshObj = AssetManager::GetAsset<Mesh>(handle);
				break;

			case ColliderType::Sphere:
				handle = AssetManager::GetPrimitiveMesh(PrimitiveType::Sphere);

				transform = glm::translate(glm::mat4(1.0f), collider.offset + trans.GetPosition()) *
					glm::toMat4(trans.GetRotation()) *
					glm::scale(glm::mat4(1.0f), glm::vec3(collider.radius * 2.));

				meshObj = AssetManager::GetAsset<Mesh>(handle);
				break;
			}

			if (!meshObj) continue;


			Renderer::Submit(meshObj, mat, transform, RenderMode::WIREFRAME);
		}
	}
}