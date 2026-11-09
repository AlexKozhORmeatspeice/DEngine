#include "dpch.h"
#include "DebugRendererSystem.h"

#include "DEngine/Scene/Components.h"
#include "DEngine/Scene/Entity.h"
#include "DEngine/Renderer/Renderer.h"

#include "DEngine/Asset/AssetManager.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"

namespace DEngine
{
	namespace
	{
		// Unit primitives are authored around the origin (cube and sphere span -0.5..0.5).
		// Collider size and radius are in the entity's local space, so the entity scale is applied here.
		glm::mat4 ColliderMatrix(const TransformComponent& trans, const glm::vec3& localOffset, const glm::vec3& localExtent)
		{
			return glm::translate(glm::mat4(1.0f), trans.GetPosition())
				* glm::toMat4(trans.GetRotation())
				* glm::translate(glm::mat4(1.0f), localOffset)
				* glm::scale(glm::mat4(1.0f), localExtent * trans.GetScale());
		}
	}

	void DebugRendererSystem::OnRender(const Timestep& ts, Scene* scene)
	{
		(void)ts;

		auto comps = scene->View<ColliderComponent, TransformComponent, DebugComponent>();
		Ref<Material> mat = AssetManager::GetAsset<Material>(AssetManager::GetBaseRendererMaterialWireframeHandle());
		if (!mat)
			return;

		for (auto [entity, collider, trans, debug] : comps.each())
		{
			if (!debug.debugOn || collider.type == ColliderType::None)
				continue;

			Ref<Mesh> meshObj = nullptr;
			glm::mat4 transform(1.0f);

			switch (collider.type)
			{
			case ColliderType::Box:
				meshObj = AssetManager::GetAsset<Mesh>(AssetManager::GetPrimitiveMesh(PrimitiveType::Cube));
				transform = ColliderMatrix(trans, collider.offset, collider.size);
				break;

			case ColliderType::Sphere:
				meshObj = AssetManager::GetAsset<Mesh>(AssetManager::GetPrimitiveMesh(PrimitiveType::Sphere));
				transform = ColliderMatrix(trans, collider.offset, glm::vec3(collider.radius * 2.0f));
				break;

			case ColliderType::Capsule:
				meshObj = AssetManager::GetAsset<Mesh>(AssetManager::GetPrimitiveMesh(PrimitiveType::Sphere));
				transform = ColliderMatrix(trans, collider.offset, glm::vec3(collider.radius * 2.0f, collider.height, collider.radius * 2.0f));
				break;

			case ColliderType::Mesh:
			{
				Entity owner(entity, scene);
				if (owner.HasComponent<MeshRendererComponent>())
				{
					meshObj = AssetManager::GetAsset<Mesh>(owner.GetComponent<MeshRendererComponent>().meshHandle);
					transform = glm::translate(trans.GetModelMatrix(), collider.offset);
				}
				break;
			}

			default:
				break;
			}

			if (!meshObj)
				continue;

			Renderer::Submit(meshObj, mat, transform, RenderMode::WIREFRAME);
		}
	}
}
