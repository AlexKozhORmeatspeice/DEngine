#include "dpch.h"
#include "PhysicSystem.h"

#include "DEngine/Scene/Components.h"

namespace DEngine
{
	void PhysicsSystem::OnUpdate(const Timestep& ts, const Scene* scene)
	{
		auto physicsComponents = scene->View<ColliderComponent, RigidbodyComponent>();

		//TODO: Здесь короч, Кость, тебе писать нужно основной код. Воооооооот. Можешь еще всякие преколы делать
		//Для ориентира можешь смотреть как написано класс MeshRendererSystem

		for (auto [entity, collider, rigidbody] : physicsComponents.each())
		{
			//Здесь короч чист обработка всех сущностей разом со всеми компонентами
		}
	}
}
