#include "dpch.h"
#include "TransformSystem.h"

#include "DEngine/Scene/Entity.h"
#include "DEngine/Scene/Components.h"
#include "DEngine/Profiler/Profiler.h"

namespace DEngine
{
    void TransformSystem::OnUpdate(const Timestep& ts, Scene* scene)
    {
        D_PROFILE_FUNCTION();
        UpdateWorld(scene);
    }

    void TransformSystem::UpdateWorld(Scene* scene)
    {
        D_PROFILE_FUNCTION();

        auto view = scene->View<TransformComponent>();
        if (view.size() == 0) return;

        std::vector<entt::entity> dirty;
        dirty.reserve(32);
        for (auto [entity, trans] : view.each())
        {
            if (!trans.isChangedTransform)
                continue;

            trans.isChangedTransform = false;
            dirty.push_back(entity);
        }

        if (dirty.empty())
            return;

        ChildMap children;
        children.reserve(view.size());
        for (entt::entity entity : view)
        {
            const TransformComponent& transform = view.get<TransformComponent>(entity);
            if (transform.Parent != 0)
                children[transform.Parent].push_back(entity);
        }

        m_VisitState.clear();
        m_VisitState.reserve(view.size());
        m_Stack.clear();
        m_Stack.reserve(64);

        const std::unordered_set<entt::entity> dirtySet(dirty.begin(), dirty.end());
        for (entt::entity entity : dirty)
        {
            if (StateOf(entity) == VisitState::Done)
                continue;

            ComposeDirty(scene, entity, view, children, dirtySet);
        }
    }

    void TransformSystem::ComposeDirty(Scene* scene, entt::entity entity, TransformView& view, const ChildMap& children, const std::unordered_set<entt::entity>& dirty)
    {
        if (!view.contains(entity))
            return;

        m_Stack.clear();

        entt::entity current = entity;
        while (view.contains(current))
        {
            const VisitState state = StateOf(current);
            if (state == VisitState::Done)
                break;
            if (state == VisitState::Visiting)
            {
                D_CORE_ASSERT(false, "Cycle detected in transform hierarchy");
                break;
            }

            m_VisitState[current] = VisitState::Visiting;
            m_Stack.push_back(current);

            const TransformComponent& transform = view.get<TransformComponent>(current);
            const entt::entity parent = FindParentEntity(scene, transform, view);
            if (parent == entt::null)
                break;

            const VisitState parentState = StateOf(parent);
            if (parentState == VisitState::Done)
                break;
            if (parentState == VisitState::Visiting)
            {
                D_CORE_ASSERT(false, "Cycle detected in transform hierarchy");
                break;
            }
            if (dirty.find(parent) == dirty.end())
                break;

            current = parent;
        }

        entt::entity top = entt::null;
        while (!m_Stack.empty())
        {
            const entt::entity handle = m_Stack.back();
            m_Stack.pop_back();
            if (StateOf(handle) == VisitState::Done)
                continue;

            WriteWorld(scene, handle, view);
            m_VisitState[handle] = VisitState::Done;
            if (top == entt::null)
                top = handle;
        }

        if (top != entt::null)
            PropagateChildren(scene, top, view, children);
    }

    void TransformSystem::PropagateChildren(Scene* scene, entt::entity entity, TransformView& view, const ChildMap& children)
    {
        m_Stack.clear();
        m_Stack.push_back(entity);

        while (!m_Stack.empty())
        {
            const entt::entity handle = m_Stack.back();
            m_Stack.pop_back();

            const uint64_t uuid = scene->GetUUID(handle);
            if (uuid == 0)
                continue;

            const auto found = children.find(uuid);
            if (found == children.end())
                continue;

            for (entt::entity child : found->second)
            {
                if (!view.contains(child))
                    continue;

                const VisitState state = StateOf(child);
                if (state == VisitState::Done || state == VisitState::Visiting)
                    continue;

                WriteWorld(scene, child, view);
                m_VisitState[child] = VisitState::Done;
                m_Stack.push_back(child);
            }
        }
    }

    void TransformSystem::WriteWorld(Scene* scene, entt::entity entity, TransformView& view)
    {
        TransformComponent& transform = view.get<TransformComponent>(entity);
        const entt::entity parent = FindParentEntity(scene, transform, view);
        if (parent != entt::null && StateOf(parent) != VisitState::Visiting)
            transform.world = view.get<TransformComponent>(parent).world * transform.trans;
        else
            transform.world = transform.trans;
    }

    entt::entity TransformSystem::FindParentEntity(Scene* scene, const TransformComponent& transform, TransformView& view) const
    {
        if (transform.Parent == 0)
            return entt::null;

        Entity parent = scene->FindEntity(transform.Parent);
        if (!parent || !parent.HasComponent<TransformComponent>())
            return entt::null;

        const entt::entity handle = parent.GetHandle();
        if (!view.contains(handle))
            return entt::null;

        return handle;
    }

    TransformSystem::VisitState TransformSystem::StateOf(entt::entity entity) const
    {
        const auto found = m_VisitState.find(entity);
        if (found == m_VisitState.end())
            return VisitState::Unvisited;

        return found->second;
    }
}