// ColliderComponent.cpp
#include "dpch.h"
#include "ColliderComponent.h"

#include <yaml-cpp/yaml.h>

namespace DEngine
{
	void ColliderComponent::Serialize(YAML::Emitter& out) const
	{
		out << YAML::Key << "Type" << YAML::Value << static_cast<uint16_t>(type);
		out << YAML::Key << "Size" << YAML::Value << size;
		out << YAML::Key << "Offset" << YAML::Value << offset;
		out << YAML::Key << "Radius" << YAML::Value << radius;
		out << YAML::Key << "Height" << YAML::Value << height;
		out << YAML::Key << "IsTrigger" << YAML::Value << isTrigger;
	}

	bool ColliderComponent::Deserialize(const YAML::Node& node)
	{
		if (node["Type"])
		{
			type = static_cast<ColliderType>(node["Type"].as<uint16_t>());
		}

		if (node["Size"])
		{
			size = node["Size"].as<glm::vec3>();
		}

		if (node["Offset"])
		{
			offset = node["Offset"].as<glm::vec3>();
		}

		if (node["Radius"])
		{
			radius = node["Radius"].as<float>();
		}

		if (node["Height"])
		{
			height = node["Height"].as<float>();
		}

		if (node["IsTrigger"])
		{
			isTrigger = node["IsTrigger"].as<bool>();
		}

		return true;
	}
}