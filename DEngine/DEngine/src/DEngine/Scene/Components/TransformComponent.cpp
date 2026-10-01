#include "dpch.h"
#include "TransformComponent.h"

#include "DEngine/Utils/YamlHelper.h"

namespace DEngine
{
    const std::string TRANS_COMP_NAME = "Transform_Component";
    const std::string TRANS_COMP_POS = "Pos";
    const std::string TRANS_COMP_ROT = "Rot";
    const std::string TRANS_COMP_SCALE = "Scale";

    // ------------------------------------------------------------
    //  Служебное: пометить кэш эйлеров как грязный
    // ------------------------------------------------------------
    static inline void MarkEulerDirty(glm::vec3& euler, bool& dirty)
    {
        dirty = true;
    }

    // ------------------------------------------------------------
    //  Конструкторы
    // ------------------------------------------------------------
    TransformComponent::TransformComponent(const glm::mat4& _trans)
        : trans(_trans)
    {
        // Кэш эйлеров построим лениво — при первом GetRotationEuler()
        m_RotationEulerDirty = true;
    }

    // ------------------------------------------------------------
    //  Position
    // ------------------------------------------------------------
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

    // ------------------------------------------------------------
    //  Rotation (quat)
    // ------------------------------------------------------------
    void TransformComponent::SetRotation(const glm::quat& rotation)
    {
        glm::vec3 scale = GetScale();
        glm::vec3 position = GetPosition();

        trans = glm::translate(glm::mat4(1.0f), position) *
            glm::toMat4(rotation) *
            glm::scale(glm::mat4(1.0f), scale);

        m_RotationEulerDirty = true;
    }

    // Прямая запись — БЕЗ чтения position/scale через Get*().
    // Использует уже имеющуюся матрицу, переписывает только блок 3x3 вращения.
    // Это критично для физики: частые вызовы не должны пересобирать
    // translate*rotate*scale заново и терять точность.
    void TransformComponent::SetRotationDirect(const glm::quat& rotation)
    {
        glm::mat4 rotMat = glm::toMat4(rotation);

        // Сохраняем scale из текущей матрицы (длины столбцов)
        float sx = glm::length(glm::vec3(trans[0]));
        float sy = glm::length(glm::vec3(trans[1]));
        float sz = glm::length(glm::vec3(trans[2]));

        // Пишем вращение * scale в верхние 3x3
        for (int c = 0; c < 3; ++c)
            for (int r = 0; r < 3; ++r)
                trans[c][r] = rotMat[c][r];

        trans[0] *= sx;
        trans[1] *= sy;
        trans[2] *= sz;

        // Позиция (4-й столбец) не трогается — уже корректна.

        m_RotationEulerDirty = true;
    }

    void TransformComponent::SetRotationEuler(const glm::vec3& eulerDegrees)
    {
        glm::quat rotation = glm::quat(glm::radians(eulerDegrees));
        SetRotation(rotation);
        m_RotationEuler = eulerDegrees;
        m_RotationEulerDirty = false;
    }

    glm::quat TransformComponent::GetRotation() const
    {
        // Извлекаем кватернион прямо из матрицы, БЕЗ эйлеров.
        // Нормируем, чтобы убрать scale.
        glm::mat3 rotMat(
            glm::normalize(glm::vec3(trans[0])),
            glm::normalize(glm::vec3(trans[1])),
            glm::normalize(glm::vec3(trans[2]))
        );
        return glm::normalize(glm::quat_cast(rotMat));
    }

    glm::vec3 TransformComponent::GetRotationEuler() const
    {
        // Ленивая пересборка кэша
        if (m_RotationEulerDirty)
        {
            glm::quat q = GetRotation();
            m_RotationEuler = glm::degrees(glm::eulerAngles(q));
            m_RotationEulerDirty = false;
        }
        return m_RotationEuler;
    }

    // ------------------------------------------------------------
    //  Scale
    // ------------------------------------------------------------
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

    // ------------------------------------------------------------
    //  Rotate helpers
    // ------------------------------------------------------------
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

    // ------------------------------------------------------------
    //  Basis vectors
    // ------------------------------------------------------------
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

    // ------------------------------------------------------------
    //  Serialization
    // ------------------------------------------------------------
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
            SetPosition(node[TRANS_COMP_POS].as<glm::vec3>());

        if (node[TRANS_COMP_ROT])
            SetRotationEuler(node[TRANS_COMP_ROT].as<glm::vec3>());

        if (node[TRANS_COMP_SCALE])
            SetScale(node[TRANS_COMP_SCALE].as<glm::vec3>());

        return true;
    }
}