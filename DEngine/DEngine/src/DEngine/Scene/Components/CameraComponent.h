#pragma once

#include "DEngine/Scene/Component.h"

namespace DEngine
{
	enum class CameraProjectionType : uint16_t
	{
		Perspective = 0,
		Orthographic = 1
	};

	struct CameraComponent : public Component
	{
		CameraProjectionType projectionType = CameraProjectionType::Perspective;
		bool primary = true;
		float fov = 60.0f;
		float orthoSize = 10.0f;
		float nearClip = 0.1f;
		float farClip = 3000.0f;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual bool Deserialize(const YAML::Node& node) override;

		DECLARE_COMPONENT(CameraComponent);
	};
}
