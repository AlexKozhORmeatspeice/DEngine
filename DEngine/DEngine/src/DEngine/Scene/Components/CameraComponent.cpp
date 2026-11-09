#include "dpch.h"
#include "CameraComponent.h"

namespace DEngine
{
	void CameraComponent::Serialize(YAML::Emitter& out) const
	{
		out << YAML::Key << "Projection" << YAML::Value << static_cast<int>(projectionType);
		out << YAML::Key << "Primary" << YAML::Value << primary;
		out << YAML::Key << "FOV" << YAML::Value << fov;
		out << YAML::Key << "OrthoSize" << YAML::Value << orthoSize;
		out << YAML::Key << "Near" << YAML::Value << nearClip;
		out << YAML::Key << "Far" << YAML::Value << farClip;
	}

	bool CameraComponent::Deserialize(const YAML::Node& node)
	{
		if (node["Projection"])
			projectionType = static_cast<CameraProjectionType>(node["Projection"].as<int>());

		if (node["Primary"])
			primary = node["Primary"].as<bool>();

		if (node["FOV"])
			fov = node["FOV"].as<float>();

		if (node["OrthoSize"])
			orthoSize = node["OrthoSize"].as<float>();

		if (node["Near"])
			nearClip = node["Near"].as<float>();

		if (node["Far"])
			farClip = node["Far"].as<float>();

		return true;
	}
}
