#include "dpch.h"
#include "TagComponent.h"

namespace DEngine
{
	TagComponent::TagComponent(const std::string& tag)
		: Tag(tag) {}

	void TagComponent::Serialize(YAML::Emitter& out) const
	{
		out << YAML::Key << "Tag" << YAML::Value << Tag;
	}

	bool TagComponent::Deserialize(const YAML::Node& node)
	{
		if (node["Tag"])
		{
			Tag = node["Tag"].as<std::string>();
			return true;
		}
		return false;
	}
}