#pragma once

#define GLM_ENABLE_EXPERIMENTAL 

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"

#include "DEngine/Scene/Component.h"

namespace DEngine
{
    struct TransformComponent : public Component
    {
        glm::mat4 trans = glm::mat4(1.0f);

        glm::vec3 m_RotationEuler = glm::vec3(0.0f);

        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const glm::mat4& _trans);

        void SetPosition(const glm::vec3& position);

        glm::vec3 GetPosition() const;

        void SetRotation(const glm::quat& rotation);
        void SetRotationEuler(const glm::vec3& eulerDegrees);
        glm::quat GetRotation() const;
        glm::vec3 GetRotationEuler() const;

        void SetScale(const glm::vec3& scale);
        glm::vec3 GetScale() const;

        void Rotate(float angle, const glm::vec3& axis);

        void RotateGlobal(float angle, const glm::vec3& axis);

        glm::mat4 GetModelMatrix() const;

        glm::vec3 GetForward() const;
        glm::vec3 GetRight() const;
        glm::vec3 GetUp() const;

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual bool Deserialize(const YAML::Node& node) override;
		DECLARE_COMPONENT(TransformComponent);
    };
}