#pragma once

#include "string"
#include "yaml-cpp/yaml.h"

#include "DEngine/Scene/Component.h"

namespace DEngine
{
	struct DebugComponent : public Component
	{
		bool debugOn = false;

		DebugComponent() = default;
		DebugComponent(const DebugComponent&) = default;
		DebugComponent(const std::string& tag);

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual bool Deserialize(const YAML::Node& node) override;

		DECLARE_COMPONENT(DebugComponent);
	};
}
