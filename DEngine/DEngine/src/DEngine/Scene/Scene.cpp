#include "dpch.h"
#include "Scene.h"

#include "Entity.h"
#include "Components.h"
#include "DEngine/Scene/System.h"
#include "DEngine/Scene/Systems.h"

namespace DEngine
{
    Scene::Scene()
    {
        AddSystem(std::make_shared<MeshRendererSystem>());
    }

    Scene::~Scene()
    {
        m_Entities.clear();
    }

    Entity Scene::CreateEntity(const std::string& name)
    {
        entt::entity handle = m_Registry.create();
        UUID uuid;

        m_Entities.emplace_back(handle, this);

        Entity& entity = m_Entities[m_Entities.size() - 1];
        entity.SetUUID(uuid);

        entity.AddComponent<TransformComponent>();

        auto& tag = entity.AddComponent<TagComponent>(name);
        tag.Tag = name.empty() ? "Entity" : name;

        return entity;
    }

    Entity Scene::CreateEmptyEntity()
    {
        entt::entity handle = m_Registry.create();
        m_Entities.emplace_back(handle, this);

        Entity& entity = m_Entities[m_Entities.size() - 1];

        return entity;
    }

    void Scene::DestroyEntity(Entity entity)
    {
        m_Registry.destroy(entity.GetHandle());
        auto it = std::find_if(m_Entities.begin(), m_Entities.end(),
            [&entity](const Entity& e) { return e == entity; });
        if (it != m_Entities.end())
            m_Entities.erase(it);
    }

    void Scene::AddSystem(std::shared_ptr<System> system)
    {
        for (const auto& sys : m_Systems)
        {
            if (typeid(*sys) == typeid(*system))
            {
                D_CORE_WARN("System is already added to scene");
                return;
            }
        }

        m_Systems.push_back(system);
    }

    void Scene::OnUpdate(const Timestep& ts)
    {
        for (auto system : m_Systems)
        {
            system->OnUpdate(ts, this);
        }
    }

    void Scene::OnRender(const Timestep& ts)
    {
        for (auto system : m_Systems)
        {
            system->OnRender(ts, this);
        }
    }
}