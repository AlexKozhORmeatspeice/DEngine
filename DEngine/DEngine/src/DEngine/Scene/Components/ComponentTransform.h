#pragma once

#define GLM_ENABLE_EXPERIMENTAL 

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"

namespace DEngine
{
    struct TransformComponent
    {
        glm::mat4 trans = glm::mat4(1.0f);

        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const glm::mat4& _trans)
            : trans(_trans) {}

        void SetPosition(const glm::vec3& position)
        {
            trans[3][0] = position.x;
            trans[3][1] = position.y;
            trans[3][2] = position.z;
        }

        glm::vec3 GetPosition() const
        {
            return { trans[3][0], trans[3][1], trans[3][2] };
        }

        void SetRotation(const glm::quat& rotation)
        {
            glm::vec3 scale = GetScale();
            glm::vec3 position = GetPosition();
            trans = glm::translate(glm::mat4(1.0f), position) * 
                    glm::toMat4(rotation) * 
                    glm::scale(glm::mat4(1.0f), scale);
        }

        glm::quat GetRotation() const
        {
            return glm::quat_cast(trans);
        }

        void SetScale(const glm::vec3& scale)
        {
            glm::vec3 position = GetPosition();
            glm::quat rotation = GetRotation();
            trans = glm::translate(glm::mat4(1.0f), position) * 
                    glm::toMat4(rotation) * 
                    glm::scale(glm::mat4(1.0f), scale);
        }

        glm::vec3 GetScale() const
        {
            return { glm::length(glm::vec3(trans[0])),
                     glm::length(glm::vec3(trans[1])),
                     glm::length(glm::vec3(trans[2])) };
        }

        // Локальное вращение (вокруг собственных осей)
        void Rotate(float angle, const glm::vec3& axis)
        {
            glm::quat currentRotation = GetRotation();
            glm::quat deltaRotation = glm::angleAxis(glm::radians(angle), glm::normalize(axis));
            SetRotation(deltaRotation * currentRotation);
        }

        // Глобальное вращение (вокруг мировых осей)
        void RotateGlobal(float angle, const glm::vec3& axis)
        {
            glm::quat currentRotation = GetRotation();
            glm::quat deltaRotation = glm::angleAxis(glm::radians(angle), glm::normalize(axis));
            SetRotation(currentRotation * deltaRotation);
        }

        glm::mat4 GetModelMatrix() const
        {
            return trans;
        }

        glm::vec3 GetForward() const
        {
            return glm::normalize(glm::vec3(trans * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)));
        }

        glm::vec3 GetRight() const
        {
            return glm::normalize(glm::vec3(trans * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)));
        }

        glm::vec3 GetUp() const
        {
            return glm::normalize(glm::vec3(trans * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)));
        }
    };
}