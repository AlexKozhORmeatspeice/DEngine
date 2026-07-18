#pragma once

#include "string"
#include "yaml-cpp/yaml.h"

#include "DEngine/Scene/Component.h"

namespace DEngine
{
	struct TagComponent : public Component
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag);

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual bool Deserialize(const YAML::Node& node) override;
		DECLARE_COMPONENT(TagComponent);
	};
}
