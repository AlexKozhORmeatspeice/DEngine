#pragma once

#include <unordered_map>
#include <memory>
#include <functional>
#include "DEngine/Scene/Component.h"

namespace DEngine
{
    class ComponentFactory
    {
    public:
        using ComponentCreator = std::function<std::unique_ptr<Component>()>;

        static ComponentFactory& Get();

        std::unique_ptr<Component> CreateComponent(const std::string& typeName) const;

        void RegisterComponent(const std::string& typeName, ComponentCreator creator);
        bool IsComponentRegistered(const std::string& typeName) const;

    private:
        std::unordered_map<std::string, ComponentCreator> m_Creators;
    };

    template<typename T>
    struct ComponentRegistrator
    {
        ComponentRegistrator()
        {
            ComponentFactory::Get().RegisterComponent(
                T().GetTypeName(),
                []() { return std::make_unique<T>(); }
            );
        }
    };
}