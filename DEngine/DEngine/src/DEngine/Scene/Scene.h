#pragma once

#include "entt.hpp"
#include "DEngine/Core/Timestep.h"
#include "DEngine/Events/Event.h"
#include "DEngine/Asset/Asset.h"

#include <memory>
#include <unordered_map>
#include <vector>

namespace DEngine
{
    class System;
    class Entity;

    class Scene : public Asset
    {
    public:
        Scene();
        ~Scene();

        Entity CreateEntity(const std::string& name = "");
        void DestroyEntity(Entity& entity);
        Entity CreateEmptyEntity();

        Entity FindEntity(uint64_t uuid) const;
        Entity GetEntity(entt::entity handle) const;
        uint64_t GetUUID(entt::entity handle) const;
        bool Contains(uint64_t uuid) const;
        bool SetParent(Entity child, Entity parent);
        void SetActiveSelf(Entity entity, bool active);
        void RebuildActivity();
        bool IsActive(const Entity& entity) const;

        template<typename... Components>
        auto View()
        {
            return m_Registry.view<Components...>();
        }

        template<typename... Components>
        auto View() const
        {
            return m_Registry.view<Components...>();
        }

        void OnUpdate(const Timestep& ts);
        void OnRender(const Timestep& ts);

        template<typename T>
        std::shared_ptr<T> GetSystem()
        {
            for (auto& system : m_Systems)
            {
                if (auto typed = std::dynamic_pointer_cast<T>(system))
                    return typed;
            }
            return nullptr;
        }

        const std::vector<Entity>& GetAllEntities() const { return m_Entities; }

        entt::registry& GetRegistry() { return m_Registry; }
        const entt::registry& GetRegistry() const { return m_Registry; }

        static AssetType GetStaticType() { return AssetType::Scene; }
        virtual AssetType GetType() const override { return GetStaticType(); }
    private:
        void AddSystem(std::shared_ptr<System> system);
        void RefreshActivity(Entity entity);
        void PropagateActivityDown(const Entity& node);

        std::vector<std::shared_ptr<System>> m_Systems;
        std::vector<Entity> m_Entities;
        entt::registry m_Registry;
        std::unordered_map<entt::entity, uint64_t> m_UUIDByEntity;
        std::unordered_map<uint64_t, entt::entity> m_EntityByUUID;

        friend class Entity;
        friend class ScenePanel;
        friend class SceneSerializer;
    };
}