#pragma once

#include "yaml-cpp/yaml.h"
#include "DEngine/Utils/YamlHelper.h"
#include <string>
#include "memory"

#define DECLARE_COMPONENT(ComponentType) \
    static std::string GetStaticTypeName() { return #ComponentType; } \
    virtual std::string GetTypeName() const override { return GetStaticTypeName(); } \
    virtual std::unique_ptr<Component> Clone() const override { return std::make_unique<ComponentType>(*this); } \
    virtual void CopyFrom(const Component& other) override \
    { \
        if (const auto* otherComp = dynamic_cast<const ComponentType*>(&other)) \
        { \
            *this = *otherComp; \
        } \
    }

namespace DEngine
{

    struct Component
    {
        virtual ~Component() = default;
        
        virtual void Serialize(YAML::Emitter& out) const = 0;
        virtual bool Deserialize(const YAML::Node& node) = 0;
        
        virtual std::string GetTypeName() const = 0;
        
        virtual std::unique_ptr<Component> Clone() const = 0;
        virtual void CopyFrom(const Component& other) = 0;
    };
}