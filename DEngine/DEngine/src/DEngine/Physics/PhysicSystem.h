#pragma once

#include "DEngine/Scene/Components/ColliderComponent.h"
#include "DEngine/Scene/Components/RigidbodyComponent.h"
#include "DEngine/Scene/System.h"
#include "glm/glm.hpp"

namespace DEngine
{
	class PhysicsSystem : public System
	{
	public:
		PhysicsSystem() {}
		virtual ~PhysicsSystem() {}

		virtual void Start() override {}

		glm::mat3 GetInertia(ColliderComponent collider, RigidbodyComponent rigidbody);

		// Применить крутящий момент к телу (в мировых осях).
		// Можно вызывать из игрового кода/скриптов до OnUpdate.
		static void ApplyTorque(RigidbodyComponent& rigidbody, const glm::vec3& torque);

		virtual void OnUpdate(const Timestep& ts, const Scene* scene) override;
		virtual void OnRender(const Timestep& ts, const Scene* scene) override {}
		virtual void Shutdown() override {}
	};
}