#pragma once

#include "DEngine/Scene/System.h"
#include "DEngine/Scene/Components.h"
#include "DEngine/Core/Timestep.h"

#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <utility>   // std::declval

#include "entt.hpp"

namespace DEngine
{
    class TransformSystem : public System
    {
    public:
        virtual void Start() override {}
        void OnUpdate(const Timestep& ts, Scene* scene) override;
        void OnRender(const Timestep& ts, Scene* scene) override {}
        virtual void Shutdown() override {}

    private:
        enum class VisitState : uint8_t
        {
            Unvisited,
            Visiting,
            Done
        };

        using TransformView = decltype(std::declval<Scene*>()->View<TransformComponent>());

        using ChildMap = std::unordered_map<uint64_t, std::vector<entt::entity>>;

        void UpdateWorld(Scene* scene);
        void ComposeDirty(Scene* scene, entt::entity entity, TransformView& view, const ChildMap& children, const std::unordered_set<entt::entity>& dirty);
        void PropagateChildren(Scene* scene, entt::entity entity, TransformView& view, const ChildMap& children);
        void WriteWorld(Scene* scene, entt::entity entity, TransformView& view);
        entt::entity FindParentEntity(Scene* scene, const TransformComponent& transform, TransformView& view) const;
        VisitState StateOf(entt::entity entity) const;

        std::unordered_map<entt::entity, VisitState> m_VisitState;
        std::vector<entt::entity> m_Stack;
    };
}