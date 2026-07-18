#include "dpch.h"
#include "ComponentMeshRenderer.h"

namespace DEngine
{
    void MeshRendererComponent::Serialize(YAML::Emitter& out) const
    {
        out << YAML::Key << "MeshHandle" << YAML::Value << meshHandle;
        out << YAML::Key << "MaterialHandle" << YAML::Value << materialHandle;
    }

    bool MeshRendererComponent::Deserialize(const YAML::Node& node)
    {
        if (node["MeshHandle"])
        {
            meshHandle = node["MeshHandle"].as<uint64_t>();
        }

        if (node["MaterialHandle"])
        {
            materialHandle = node["MaterialHandle"].as<uint64_t>();
        }

        return true;
    }
}