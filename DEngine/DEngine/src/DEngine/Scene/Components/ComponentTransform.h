#pragma once

#include "glm/glm.hpp"

namespace DEngine
{
	struct TransformComponent
	{
		glm::mat4 trans = glm::mat4(1.0f);

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::mat4& _trans)
			: trans(_trans) {}
	};
}
