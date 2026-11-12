#pragma once

#include "DEngine/Scene/System.h"
#include "DEngine/Scene/Components.h"
#include "DEngine/Core/Timestep.h"

#include <cstdint>
#include <unordered_map>
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

        void UpdateWorld(Scene* scene);

        void ComposeIterative(Scene* scene, entt::entity root, TransformView& view);

        std::unordered_map<entt::entity, VisitState> m_VisitState;
        std::vector<entt::entity> m_Stack;
    };
}