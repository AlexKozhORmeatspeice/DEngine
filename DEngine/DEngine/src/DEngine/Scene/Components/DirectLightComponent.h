#pragma once
#pragma once

#include "DEngine/Core.h"
#include "glm/glm.hpp"

namespace DEngine
{
	struct DirectLightComponent
	{
		glm::vec3 color;
		float intensity;

		DirectLightComponent() = default;
		DirectLightComponent(const DirectLightComponent&) = default;
		DirectLightComponent(const glm::vec3& _color, float _intensity)
			: color(_color), intensity(_intensity)
		{ }
	};
}

