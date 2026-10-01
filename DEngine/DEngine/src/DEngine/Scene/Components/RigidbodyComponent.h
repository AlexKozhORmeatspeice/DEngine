#pragma once

#include "DEngine/Core.h"
#include "DEngine/Asset/Asset.h"

#include "DEngine/Scene/Component.h"
#include "glm/glm.hpp"

namespace DEngine
{
	struct RigidbodyComponent : public Component
	{
		float mass = 1.0f;
		glm::vec3 velocity = glm::vec3(0.0f);
		glm::vec3 acceleration = glm::vec3(0.0f);
		glm::vec3 force = glm::vec3(0.0f);
		glm::vec3 angVel = glm::vec3(0.0f);
		glm::vec3 angAcc = glm::vec3(0.0f);
		glm::vec3 torque = glm::vec3(0.0f);
		glm::mat3 inertia = glm::mat3(1.0f);
		glm::mat3 inertia_inv = glm::inverse(inertia);
		bool useGravity = true;
		bool isKinematic = false;

		RigidbodyComponent() = default;
		
		RigidbodyComponent(const RigidbodyComponent& comp)
		{
			mass = comp.mass;
			velocity = comp.velocity;
			acceleration = comp.acceleration;
			force = comp.force;
			useGravity = comp.useGravity;
			isKinematic = comp.isKinematic;
		}

		RigidbodyComponent(float _mass)
			: mass(_mass)
		{
		}

		RigidbodyComponent(float _mass, const glm::vec3& _velocity)
			: mass(_mass), velocity(_velocity)
		{
		}

		RigidbodyComponent(float _mass, const glm::vec3& _velocity, const glm::vec3& _acceleration)
			: mass(_mass), velocity(_velocity), acceleration(_acceleration)
		{
		}

		void ApplyForce(const glm::vec3& _force)
		{
			force += _force;
		}

		void ApplyTorque(const glm::vec3& _torque)
		{
			torque += _torque;
		}

		void ClearForces()
		{
			force = glm::vec3(0.0f);
		}

		void ClearTorque()
		{
			torque = glm::vec3(0.0f);
		}

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual bool Deserialize(const YAML::Node& node) override;

		DECLARE_COMPONENT(RigidbodyComponent);
	};
}