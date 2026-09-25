#include "dpch.h"
#include "PhysicSystem.h"

#include "DEngine/Scene/Components.h"

namespace DEngine
{
	void PhysicsSystem::OnUpdate(const Timestep& ts, const Scene* scene)
	{
		auto physicsComponents = scene->View<ColliderComponent, RigidbodyComponent, TransformComponent>();

		//TODO: Здесь короч, Кость, тебе писать нужно основной код. Воооооооот. Можешь еще всякие преколы делать
		//Для ориентира можешь смотреть как написано класс MeshRendererSystem

		for (auto [entity, collider, rigidbody, transform] : physicsComponents.each())
		{	
			//Здесь короч чист обработка всех сущностей разом со всеми компонентами

			if (rigidbody.isKinematic)
			{
				continue;
			}

			if (rigidbody.useGravity)
			{
				rigidbody.force = glm::vec3(0, -9.81 * rigidbody.mass, 0);
			}

			float dt = ts.GetSeconds();
			rigidbody.acceleration += rigidbody.force / rigidbody.mass;
			rigidbody.velocity += rigidbody.acceleration * dt;
			glm::vec3 dPos = rigidbody.velocity * dt;
			transform.SetPosition(transform.GetPosition() + dPos);

			glm::vec3 curPos = transform.GetPosition();
			if (curPos[1] <= -40.0)
			{
				curPos[1] = -40.0;
				transform.SetPosition(curPos);
				rigidbody.velocity = -rigidbody.velocity * 0.85f;
			}
		}
	}
}
