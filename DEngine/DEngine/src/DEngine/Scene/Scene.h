#pragma once

#include "entt.hpp"
#include "DEngine/Core/Timestep.h"
#include "DEngine/Events/Event.h"
#include "DEngine/Asset/Asset.h"
#include <memory>
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
        Entity CreateEmptyEntity();

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

        void DestroyEntity(Entity entity);

        void OnUpdate(const Timestep& ts);
        void OnRender(const Timestep& ts);

        const std::vector<Entity>& GetAllEntities() const { return m_Entities; }

        entt::registry& GetRegistry() { return m_Registry; }
        const entt::registry& GetRegistry() const { return m_Registry; }

		static AssetType GetStaticType() { return AssetType::Scene; }
		virtual AssetType GetType() const override { return GetStaticType(); }
    private:
        void AddSystem(std::shared_ptr<System> system);

        std::vector<std::shared_ptr<System>> m_Systems;
        std::vector<Entity> m_Entities;
        entt::registry m_Registry;

        friend class Entity;
        friend class ScenePanel;
        friend class SceneSerializer;
    };
}