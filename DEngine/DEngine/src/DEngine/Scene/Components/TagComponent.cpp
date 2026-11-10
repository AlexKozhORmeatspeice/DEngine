#include "dpch.h"
#include "TagComponent.h"

namespace DEngine
{
	TagComponent::TagComponent(const std::string& tag)
		: Tag(tag) {}

	void TagComponent::Serialize(YAML::Emitter& out) const
	{
		out << YAML::Key << "Tag" << YAML::Value << Tag;
		out << YAML::Key << "Active" << YAML::Value << activeSelf;
	}

	bool TagComponent::Deserialize(const YAML::Node& node)
	{
		if (node["Tag"])
		{
			Tag = node["Tag"].as<std::string>();
			if (node["Active"])
				activeSelf = node["Active"].as<bool>();
			return true;
		}
		return false;
	}
}