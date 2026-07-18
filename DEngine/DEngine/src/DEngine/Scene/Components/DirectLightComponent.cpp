#include "dpch.h"
#include "DirectLightComponent.h"

namespace DEngine
{
    void DirectLightComponent::Serialize(YAML::Emitter& out) const
    {
        out << YAML::Key << "Color" << YAML::Value << color;
        out << YAML::Key << "Intensity" << YAML::Value << intensity;
    }

    bool DirectLightComponent::Deserialize(const YAML::Node& node)
    {
        if (node["Color"])
        {
            color = node["Color"].as<glm::vec3>();
        }

        if (node["Intensity"])
        {
            intensity = node["Intensity"].as<float>();
        }

        return true;
    }
}