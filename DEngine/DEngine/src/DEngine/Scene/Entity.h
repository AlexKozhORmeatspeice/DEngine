#pragma once

#include "DEngine/Core.h"
#include "Scene.h"
#include "entt.hpp"
#include "DEngine/Scene/Component.h"
#include <unordered_map>
#include <memory>

#include "Components.h"

namespace DEngine
{
    class Entity
    {
    public:
        Entity() : m_Handle(entt::null), m_Scene(nullptr) {}
        Entity(entt::entity handle, Scene* scene) : m_Handle(handle), m_Scene(scene) {}
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

        operator bool() const { return m_Handle != entt::null && m_Scene != nullptr; }
        bool operator==(const Entity& other) const { return m_Handle == other.m_Handle && m_Scene == other.m_Scene; }
        bool operator!=(const Entity& other) const { return m_Handle != other.m_Handle || m_Scene != other.m_Scene; }
        operator uint32_t() const { return static_cast<uint32_t>(m_Handle); }

        entt::entity GetHandle() const { return m_Handle; }
        Scene* GetScene() const { return m_Scene; }

    private:
        entt::entity m_Handle{ entt::null };
        Scene* m_Scene = nullptr;
        uint64_t m_UUID = 0;
    };
}