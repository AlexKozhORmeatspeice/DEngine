#include "dpch.h"
#include "RigidbodyComponent.h"

#include <yaml-cpp/yaml.h>

namespace DEngine
{
    void RigidbodyComponent::Serialize(YAML::Emitter& out) const
    {
        out << YAML::Key << "Mass" << YAML::Value << mass;
        out << YAML::Key << "Velocity" << YAML::Value << velocity;
        out << YAML::Key << "Acceleration" << YAML::Value << acceleration;
        out << YAML::Key << "Force" << YAML::Value << force;
        out << YAML::Key << "UseGravity" << YAML::Value << useGravity;
        out << YAML::Key << "IsKinematic" << YAML::Value << isKinematic;
    }

    bool RigidbodyComponent::Deserialize(const YAML::Node& node)
    {
        if (node["Mass"])
        {
            mass = node["Mass"].as<float>();
        }

        if (node["Velocity"])
        {
            velocity = node["Velocity"].as<glm::vec3>();
        }

        if (node["Acceleration"])
        {
            acceleration = node["Acceleration"].as<glm::vec3>();
        }

        if (node["Force"])
        {
            force = node["Force"].as<glm::vec3>();
        }

        if (node["UseGravity"])
        {
            useGravity = node["UseGravity"].as<bool>();
        }

        if (node["IsKinematic"])
        {
            isKinematic = node["IsKinematic"].as<bool>();
        }

        return true;
    }
}