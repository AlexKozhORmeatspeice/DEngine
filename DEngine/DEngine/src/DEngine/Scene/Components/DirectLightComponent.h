#pragma once
#pragma once

#include "DEngine/Core.h"
#include "glm/glm.hpp"

#include "DEngine/Scene/Component.h"

namespace DEngine
{
	struct DirectLightComponent : public Component
	{
		glm::vec3 color;
		float intensity;

		DirectLightComponent() = default;
		DirectLightComponent(const DirectLightComponent&) = default;
		DirectLightComponent(const glm::vec3& _color, float _intensity)
			: color(_color), intensity(_intensity)
		{ }

		//TODO: доделать сериализацию
		virtual void Serialize(YAML::Emitter& out) const override {}
		virtual bool Deserialize(const YAML::Node& node) override { return true; }

		DECLARE_COMPONENT(DirectLightComponent);
	};
}

