#pragma once

#include "DEngine/Core/UUID.h"
#include "fstream"
#include "yaml-cpp/yaml.h"
#include "glm/glm.hpp"

namespace YAML {
    template<>
    struct convert<glm::vec2>
    {
        static Node encode(const glm::vec2& rhs);
        static bool decode(const Node& node, glm::vec2& rhs);
    };

    template<>
    struct convert<glm::vec3>
    {
        static Node encode(const glm::vec3& rhs);
        static bool decode(const Node& node, glm::vec3& rhs);
    };

    template<>
    struct convert<glm::vec4>
    {
        static Node encode(const glm::vec4& rhs);
        static bool decode(const Node& node, glm::vec4& rhs);
    };

    template<>
    struct convert<DEngine::UUID>
    {
        static Node encode(const DEngine::UUID& uuid);
        static bool decode(const Node& node, DEngine::UUID& uuid);
    };

    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v);
}