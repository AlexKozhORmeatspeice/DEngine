#pragma once

#include "DEngine/Core.h"
#include "Scene.h"
#include "DEngine/Scene/Component.h"
#include "DEngine/Scene/EntityHandle.h"     
#include <unordered_map>
#include <memory>

#include "Components.h"

namespace DEngine
{
    class Entity
    {
    public:
        Entity() : m_Handle(EntityHandle::Invalid()), m_Scene(nullptr) {}
        Entity(EntityHandle handle, Scene* scene) : m_Handle(handle), m_Scene(scene) {}
        ~Entity() = default;

        template<typename T>
        bool HasComponent() const
        {
            static_assert(std::is_base_of_v<Component, T>, "T must be a Component");
            return m_Scene && m_Scene->m_Registry.all_of<T>(m_Handle);
        }

        template<typename T, typename... Args>
        T& AddComponent(Args&&... args)
        {
            static_assert(std::is_base_of_v<Component, T>, "T must be a Component");
            D_CORE_ASSERT(!HasComponent<T>(), "Entity already has component");
            
            return m_Scene->m_Registry.emplace<T>(m_Handle, std::forward<Args>(args)...);
        }

        template<typename T>
        T& GetComponent()
        {
            static_assert(std::is_base_of_v<Component, T>, "T must be a Component");
            D_CORE_ASSERT(HasComponent<T>(), "Entity doesn't have component");
            return m_Scene->m_Registry.get<T>(m_Handle);
        }

        template<typename T>
        const T& GetComponent() const
        {
            static_assert(std::is_base_of_v<Component, T>, "T must be a Component");
            D_CORE_ASSERT(HasComponent<T>(), "Entity doesn't have component");
            return m_Scene->m_Registry.get<T>(m_Handle);
        }

        template<typename T>
        void RemoveComponent()
        {
            static_assert(std::is_base_of_v<Component, T>, "T must be a Component");
            D_CORE_ASSERT(HasComponent<T>(), "Entity doesn't have component to remove");
            m_Scene->m_Registry.remove<T>(m_Handle);
        }

        uint64_t GetUUID() const { return m_UUID; }
        void SetUUID(uint64_t uuid) { m_UUID = uuid; }

        bool operator==(const Entity& other) const { return m_Handle == other.m_Handle && m_Scene == other.m_Scene; }
        bool operator!=(const Entity& other) const { return m_Handle != other.m_Handle || m_Scene != other.m_Scene; }
        operator uint32_t() const { return static_cast<uint32_t>(m_Handle); }
        operator bool() const { return static_cast<bool>(m_Handle) && m_Scene != nullptr; }

        EntityHandle GetHandle() const { return m_Handle; }
        Scene* GetScene() const { return m_Scene; }

    private:
        EntityHandle m_Handle = EntityHandle::Invalid();
        Scene* m_Scene = nullptr;
        uint64_t m_UUID = 0;
    };
}