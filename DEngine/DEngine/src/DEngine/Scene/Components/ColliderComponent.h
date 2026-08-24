#pragma once

#include "DEngine/Core.h"
#include "DEngine/Scene/Component.h"
#include "glm/glm.hpp"

namespace DEngine
{
	enum class ColliderType : uint16_t
	{
		None = 0,
		Box,
		Sphere,
		Capsule,
		Mesh
	};

	struct ColliderComponent : public Component
	{
		ColliderType type = ColliderType::Box;
		glm::vec3 size = glm::vec3(1.0f);
		glm::vec3 offset = glm::vec3(0.0f);
		float radius = 0.5f;
		float height = 1.0f;
		bool isTrigger = false;

		ColliderComponent() = default;

		ColliderComponent(const ColliderComponent& comp)
		{
			type = comp.type;
			size = comp.size;
			offset = comp.offset;
			radius = comp.radius;
			height = comp.height;
			isTrigger = comp.isTrigger;
		}

		ColliderComponent(ColliderType _type)
			: type(_type)
		{
		}

		ColliderComponent(ColliderType _type, const glm::vec3& _size)
			: type(_type), size(_size)
		{
		}

		ColliderComponent(ColliderType _type, const glm::vec3& _size, const glm::vec3& _offset)
			: type(_type), size(_size), offset(_offset)
		{
		}

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual bool Deserialize(const YAML::Node& node) override;

		DECLARE_COMPONENT(ColliderComponent);
	};
}