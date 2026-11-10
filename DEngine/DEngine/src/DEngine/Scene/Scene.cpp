#include "dpch.h"
#include "Scene.h"

#include "Entity.h"
#include "Components.h"
#include "DEngine/Scene/System.h"
#include "DEngine/Scene/Systems.h"
#include "DEngine/Profiler/Profiler.h"

#include <unordered_map>
#include <unordered_set>


namespace DEngine
{
    Scene::Scene()
    {
        AddSystem(std::make_shared<TransformSystem>());
        AddSystem(std::make_shared<CameraSystem>());
        AddSystem(std::make_shared<MeshRendererSystem>());
        AddSystem(std::make_shared<DebugRendererSystem>());
        AddSystem(std::make_shared<PhysicsSystem>());
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

    void Scene::DestroyEntity(Entity& entity)
    {
        uint64_t uuid = entity.GetUUID();
        if (uuid == 0)
            uuid = GetUUID(entity.GetHandle());
        if (uuid != 0)
        {
            for (Entity& other : m_Entities)
            {
                if (!other.HasComponent<TransformComponent>())
                    continue;
                if (other.GetComponent<TransformComponent>().Parent == uuid)
                    other.GetComponent<TransformComponent>().Parent = 0;
            }
        }

        entt::entity handle = entity.GetHandle();
        m_UUIDByEntity.erase(handle);
        if (uuid != 0)
            m_EntityByUUID.erase(uuid);

        m_Registry.destroy(handle);
        auto it = std::find_if(m_Entities.begin(), m_Entities.end(),
            [&entity](const Entity& e) { return e == entity; });
        if (it != m_Entities.end())
            m_Entities.erase(it);

        RebuildActivity();
    }

    Entity Scene::FindEntity(uint64_t uuid) const
    {
        if (uuid == 0)
            return {};

        auto it = m_EntityByUUID.find(uuid);
        if (it == m_EntityByUUID.end())
            return {};
        return GetEntity(it->second);
    }

    Entity Scene::GetEntity(entt::entity handle) const
    {
        if (handle == entt::null || !m_Registry.valid(handle))
            return {};

        Entity entity(handle, const_cast<Scene*>(this));
        auto it = m_UUIDByEntity.find(handle);
        if (it != m_UUIDByEntity.end())
            entity.m_UUID = it->second;
        return entity;
    }

    uint64_t Scene::GetUUID(entt::entity handle) const
    {
        auto it = m_UUIDByEntity.find(handle);
        return it == m_UUIDByEntity.end() ? 0 : it->second;
    }

    bool Scene::Contains(uint64_t uuid) const
    {
        return uuid != 0 && m_EntityByUUID.find(uuid) != m_EntityByUUID.end();
    }

    bool Scene::SetParent(Entity child, Entity parent)
    {
        if (!child || !child.HasComponent<TransformComponent>())
            return false;

        if (!parent)
        {
            child.GetComponent<TransformComponent>().Parent = 0;
            RefreshActivity(child);
            return true;
        }

        if (parent == child || parent.GetUUID() == 0 || !parent.HasComponent<TransformComponent>())
            return false;

        Entity walk = parent;
        for (int guard = 0; guard < 64 && walk; ++guard)
        {
            if (walk == child)
                return false;
            uint64_t next = walk.GetComponent<TransformComponent>().Parent;
            if (next == 0)
                break;
            walk = FindEntity(next);
        }

        child.GetComponent<TransformComponent>().Parent = parent.GetUUID();
        RefreshActivity(child);
        return true;
    }

    void Scene::SetActiveSelf(Entity entity, bool active)
    {
        if (!entity || !entity.HasComponent<TagComponent>())
            return;

        entity.GetComponent<TagComponent>().activeSelf = active;
        PropagateActivityDown(entity);
    }

    bool Scene::IsActive(const Entity& entity) const
    {
        if (!entity)
            return false;
        if (entity.HasComponent<TagComponent>() && !entity.GetComponent<TagComponent>().activeSelf)
            return false;
        if (entity.HasComponent<TransformComponent>() && entity.GetComponent<TransformComponent>().unactiveByParent)
            return false;
        return true;
    }

    void Scene::RefreshActivity(Entity entity)
    {
        if (!entity)
            return;

        if (entity.HasComponent<TransformComponent>())
        {
            TransformComponent& transform = entity.GetComponent<TransformComponent>();
            bool disabledByParent = false;
            if (transform.Parent != 0)
            {
                Entity parent = FindEntity(transform.Parent);
                if (parent)
                    disabledByParent = !IsActive(parent);
            }
            transform.unactiveByParent = disabledByParent;
        }

        PropagateActivityDown(entity);
    }

    void Scene::PropagateActivityDown(const Entity& node)
    {
        if (!node || node.GetUUID() == 0)
            return;

        std::unordered_map<uint64_t, std::vector<Entity>> children;
        children.reserve(m_Entities.size());
        for (Entity entity : m_Entities)
        {
            if (!entity.HasComponent<TransformComponent>())
                continue;
            uint64_t parent = entity.GetComponent<TransformComponent>().Parent;
            if (parent != 0)
                children[parent].push_back(entity);
        }

        std::vector<Entity> stack;
        std::unordered_set<uint64_t> seen;
        seen.insert(node.GetUUID());
        stack.push_back(node);

        while (!stack.empty())
        {
            Entity current = stack.back();
            stack.pop_back();

            auto found = children.find(current.GetUUID());
            if (found == children.end())
                continue;

            const bool currentActive = IsActive(current);
            for (Entity& child : found->second)
            {
                uint64_t childId = child.GetUUID();
                if (childId == 0 || !seen.insert(childId).second)
                    continue;

                if (child.HasComponent<TransformComponent>())
                    child.GetComponent<TransformComponent>().unactiveByParent = !currentActive;

                stack.push_back(child);
            }
        }
    }

    void Scene::RebuildActivity()
    {
        std::unordered_set<uint64_t> ids;
        ids.reserve(m_Entities.size());
        for (const Entity& entity : m_Entities)
        {
            if (entity.GetUUID() != 0 && entity.HasComponent<TransformComponent>())
                ids.insert(entity.GetUUID());
        }

        std::unordered_map<uint64_t, std::vector<Entity>> children;
        std::vector<Entity> stack;
        std::unordered_set<uint64_t> seen;
        children.reserve(m_Entities.size());

        for (Entity entity : m_Entities)
        {
            if (!entity.HasComponent<TransformComponent>())
                continue;

            TransformComponent& transform = entity.GetComponent<TransformComponent>();
            uint64_t parent = transform.Parent;
            if (parent != 0 && ids.find(parent) != ids.end())
            {
                children[parent].push_back(entity);
                continue;
            }

            transform.unactiveByParent = false;
            uint64_t id = entity.GetUUID();
            if (id != 0 && seen.insert(id).second)
                stack.push_back(entity);
        }

        while (!stack.empty())
        {
            Entity current = stack.back();
            stack.pop_back();

            auto found = children.find(current.GetUUID());
            if (found == children.end())
                continue;

            const bool currentActive = IsActive(current);
            for (Entity& child : found->second)
            {
                uint64_t childId = child.GetUUID();
                if (childId == 0 || !seen.insert(childId).second)
                    continue;

                child.GetComponent<TransformComponent>().unactiveByParent = !currentActive;
                stack.push_back(child);
            }
        }
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
        D_PROFILE_FUNCTION();
        for (auto system : m_Systems)
        {
            system->OnUpdate(ts, this);
        }
    }

    void Scene::OnRender(const Timestep& ts)
    {
        D_PROFILE_FUNCTION();
        for (auto system : m_Systems)
        {
            system->OnRender(ts, this);
        }
    }
}