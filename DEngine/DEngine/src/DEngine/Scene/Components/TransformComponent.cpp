#include "dpch.h"
#include "TransformComponent.h"

#include "DEngine/Utils/YamlHelper.h"

namespace DEngine
{
    const std::string TRANS_COMP_NAME = "Transform_Component";
    const std::string TRANS_COMP_POS = "Pos";
    const std::string TRANS_COMP_ROT = "Rot";
    const std::string TRANS_COMP_SCALE = "Scale";

    TransformComponent::TransformComponent(const glm::mat4& _trans)
        : trans(_trans) 
    {
        m_RotationEuler = glm::degrees(glm::eulerAngles(glm::quat_cast(trans)));
    }

    void TransformComponent::SetPosition(const glm::vec3& position)
    {
        trans[3][0] = position.x;
        trans[3][1] = position.y;
        trans[3][2] = position.z;
    }

    glm::vec3 TransformComponent::GetPosition() const
    {
        return { trans[3][0], trans[3][1], trans[3][2] };
    }

    void TransformComponent::SetRotation(const glm::quat& rotation)
    {
        m_RotationEuler = glm::degrees(glm::eulerAngles(rotation));
        
        glm::vec3 scale = GetScale();
        glm::vec3 position = GetPosition();
        trans = glm::translate(glm::mat4(1.0f), position) * 
                glm::toMat4(rotation) * 
                glm::scale(glm::mat4(1.0f), scale);
    }

    void TransformComponent::SetRotationEuler(const glm::vec3& eulerDegrees)
    {
        m_RotationEuler = eulerDegrees;
        
        glm::quat rotation = glm::quat(glm::radians(eulerDegrees));
        glm::vec3 scale = GetScale();
        glm::vec3 position = GetPosition();
        trans = glm::translate(glm::mat4(1.0f), position) * 
                glm::toMat4(rotation) * 
                glm::scale(glm::mat4(1.0f), scale);
    }

    glm::quat TransformComponent::GetRotation() const
    {
        return glm::quat(glm::radians(m_RotationEuler));
    }

    glm::vec3 TransformComponent::GetRotationEuler() const
    {
        return m_RotationEuler;
    }

    void TransformComponent::SetScale(const glm::vec3& scale)
    {
        glm::vec3 position = GetPosition();
        glm::quat rotation = GetRotation();
        trans = glm::translate(glm::mat4(1.0f), position) * 
                glm::toMat4(rotation) * 
                glm::scale(glm::mat4(1.0f), scale);
    }

    glm::vec3 TransformComponent::GetScale() const
    {
        return { glm::length(glm::vec3(trans[0])),
                 glm::length(glm::vec3(trans[1])),
                 glm::length(glm::vec3(trans[2])) };
    }

    void TransformComponent::Rotate(float angle, const glm::vec3& axis)
    {
        glm::quat currentRotation = GetRotation();
        glm::quat deltaRotation = glm::angleAxis(glm::radians(angle), glm::normalize(axis));
        SetRotation(deltaRotation * currentRotation);
    }

    void TransformComponent::RotateGlobal(float angle, const glm::vec3& axis)
    {
        glm::quat currentRotation = GetRotation();
        glm::quat deltaRotation = glm::angleAxis(glm::radians(angle), glm::normalize(axis));
        SetRotation(currentRotation * deltaRotation);
    }

    glm::mat4 TransformComponent::GetModelMatrix() const
    {
        return trans;
    }

    glm::vec3 TransformComponent::GetForward() const
    {
        return glm::normalize(glm::vec3(trans * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)));
    }

    glm::vec3 TransformComponent::GetRight() const
    {
        return glm::normalize(glm::vec3(trans * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)));
    }

    glm::vec3 TransformComponent::GetUp() const
    {
        return glm::normalize(glm::vec3(trans * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)));
    }

    void TransformComponent::Serialize(YAML::Emitter& out) const
    {
        glm::vec3 pos = GetPosition();
        glm::vec3 rot = GetRotationEuler();
        glm::vec3 scale = GetScale();

        out << YAML::Key << TRANS_COMP_POS << YAML::Value << pos;
        out << YAML::Key << TRANS_COMP_ROT << YAML::Value << rot;
        out << YAML::Key << TRANS_COMP_SCALE << YAML::Value << scale;
    }

    bool TransformComponent::Deserialize(const YAML::Node& node)
    {
        if (node[TRANS_COMP_POS])
        {
            SetPosition(node[TRANS_COMP_POS].as<glm::vec3>());
        }

        if (node[TRANS_COMP_ROT])
        {
            SetRotationEuler(node[TRANS_COMP_ROT].as<glm::vec3>());
        }

        if (node[TRANS_COMP_SCALE])
        {
            SetScale(node[TRANS_COMP_SCALE].as<glm::vec3>());
        }

        return true;
    }
}