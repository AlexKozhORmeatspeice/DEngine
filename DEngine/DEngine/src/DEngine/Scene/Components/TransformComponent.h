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

        // Ёйлеровы углы теперь “ќЋ№ ќ  ЁЎ. „итаютс€ через GetRotationEuler(),
        // обновл€ютс€ при чтении. ¬ основной логике не участвуют.
        mutable glm::vec3 m_RotationEuler = glm::vec3(0.0f);
        mutable bool m_RotationEulerDirty = true;

        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const glm::mat4& _trans);

        void SetPosition(const glm::vec3& position);

        glm::vec3 GetPosition() const;

        // ќсновной путь дл€ вращени€ Ч работает через кватернион из матрицы.
        void SetRotation(const glm::quat& rotation);

        // ѕр€ма€ запись кватерниона без пересборки scale/position (дл€ физики).
        // ќтличие от SetRotation: не читает position/scale из trans, а берЄт
        // из уже существующей матрицы. ћеньше потерь точности при частых запис€х.
        void SetRotationDirect(const glm::quat& rotation);

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