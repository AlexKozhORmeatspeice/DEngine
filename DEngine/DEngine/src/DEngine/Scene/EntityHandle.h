#pragma once

#include <cstdint>
#include <functional>

namespace DEngine
{
    struct EntityHandle
    {
        uint32_t index = 0;
        uint32_t generation = 0;

        static constexpr EntityHandle Invalid() { return { 0, 0 }; }

        bool operator==(const EntityHandle& other) const
        {
            return index == other.index && generation == other.generation;
        }

        bool operator!=(const EntityHandle& other) const
        {
            return !(*this == other);
        }

        explicit operator bool() const
        {
            return generation != 0;
        }

        explicit operator uint32_t() const
        {
            return index;
        }
    };
}

namespace std
{
    template<>
    struct hash<DEngine::EntityHandle>
    {
        size_t operator()(const DEngine::EntityHandle& handle) const noexcept
        {
            return (static_cast<size_t>(handle.index) << 32) | handle.generation;
        }
    };
}