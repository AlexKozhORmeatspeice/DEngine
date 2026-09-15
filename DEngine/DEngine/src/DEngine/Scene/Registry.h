#pragma once

#include "EntityHandle.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <typeindex>
#include <any>
#include <cassert>
#include <functional>

namespace DEngine
{
    class Registry;

    template<typename... Components>
    class View
    {
    public:

        View(Registry& registry) : m_Registry(&registry) {}

        class Iterator
        {
        public:
            Iterator(Registry* registry, std::vector<EntityHandle>::iterator it, std::vector<EntityHandle>::iterator end)
                : m_Registry(registry), m_It(it), m_End(end)
            {
                SkipInvalid();
            }

            auto operator*() const
            {
                EntityHandle handle = *m_It;
                return std::tuple<EntityHandle, Components&...>{
                    handle,
                        m_Registry->get<Components>(handle)...
                };
            }

            Iterator& operator++()
            {
                ++m_It;
                SkipInvalid();
                return *this;
            }

            bool operator!=(const Iterator& other) const
            {
                return m_It != other.m_It;
            }

        private:
            void SkipInvalid()
            {
                while (m_It != m_End)
                {
                    EntityHandle handle = *m_It;
                    if (m_Registry->valid(handle) && (m_Registry->all_of<Components>(handle) && ...))
                        break;
                    ++m_It;
                }
            }

            Registry* m_Registry;
            std::vector<EntityHandle>::iterator m_It;
            std::vector<EntityHandle>::iterator m_End;
        };

        class EachProxy
        {
        public:
            EachProxy(Registry& registry) : m_Registry(&registry)
            {
                m_Entities = m_Registry->GetAllEntities();
            }

            Iterator begin() { return Iterator(m_Registry, m_Entities.begin(), m_Entities.end()); }
            Iterator end() { return Iterator(m_Registry, m_Entities.end(), m_Entities.end()); }

        private:
            Registry* m_Registry;
            std::vector<EntityHandle> m_Entities;
        };

        EachProxy each()
        {
            return EachProxy(*m_Registry);
        }

    private:
        Registry* m_Registry;
    };

    class Registry
    {
    public:

        struct Storage
        {
            std::any map;
            std::function<void(EntityHandle)> removeFn;
        };

        std::vector<EntityHandle> GetAllEntities() const
        {
            std::vector<EntityHandle> result;
            for (uint32_t i = 0; i < m_Generations.size(); ++i)
            {
                if (m_Generations[i] != 0)
                {
                    EntityHandle handle;
                    handle.index = i;
                    handle.generation = m_Generations[i];
                    result.push_back(handle);
                }
            }
            return result;
        }

        template<typename... Components>
        View<Components...> view()
        {
            return View<Components...>(*this);
        }

        template<typename... Components>
        View<Components...> view() const
        {
            // для const-версии пока можно так же
            return View<Components...>(const_cast<Registry&>(*this));
        }

        EntityHandle create()
        {
            EntityHandle handle;
            handle.index = m_NextIndex++;
            handle.generation = 1;

            if (handle.index >= m_Generations.size())
                m_Generations.resize(handle.index + 1, 0);

            m_Generations[handle.index] = handle.generation;
            return handle;
        }

        void destroy(EntityHandle handle)
        {
            if (!valid(handle))
                return;

            for (auto& [type, storage] : m_Storages)
            {
                if (storage.removeFn)
                    storage.removeFn(handle);
            }

            m_Generations[handle.index]++;
        }

        bool valid(EntityHandle handle) const
        {
            if (handle.index >= m_Generations.size())
                return false;

            return m_Generations[handle.index] == handle.generation && handle.generation != 0;
        }

        // ==================== Компоненты ====================

        template<typename T, typename... Args>
        T& emplace(EntityHandle handle, Args&&... args)
        {
            assert(valid(handle));

            auto& storage = GetOrCreateStorage<T>();
            auto [it, inserted] = storage.emplace(handle, T{ std::forward<Args>(args)... });
            assert(inserted && "Component already exists");
            return it->second;
        }

        template<typename T>
        T& get(EntityHandle handle)
        {
            assert(valid(handle));
            auto& storage = GetStorage<T>();
            auto it = storage.find(handle);
            assert(it != storage.end() && "Component not found");
            return it->second;
        }

        template<typename T>
        const T& get(EntityHandle handle) const
        {
            assert(valid(handle));
            const auto& storage = GetStorage<T>();
            auto it = storage.find(handle);
            assert(it != storage.end() && "Component not found");
            return it->second;
        }

        template<typename T>
        bool all_of(EntityHandle handle) const
        {
            if (!valid(handle))
                return false;

            std::type_index type = typeid(T);
            auto it = m_Storages.find(type);
            if (it == m_Storages.end())
                return false;   // хранилища ещё нет → компонента точно нет

            const auto& storage = std::any_cast<const std::unordered_map<EntityHandle, T>&>(it->second.map);
            return storage.find(handle) != storage.end();
        }

        template<typename T>
        void remove(EntityHandle handle)
        {
            if (!valid(handle))
                return;

            std::type_index type = typeid(T);
            auto it = m_Storages.find(type);
            if (it == m_Storages.end())
                return;

            auto& storage = std::any_cast<std::unordered_map<EntityHandle, T>&>(it->second.map);
            storage.erase(handle);
        }

    private:
        uint32_t m_NextIndex = 0;
        std::vector<uint32_t> m_Generations;

        // type -> map<EntityHandle, T>
        std::unordered_map<std::type_index, Storage> m_Storages;

        template<typename T>
        std::unordered_map<EntityHandle, T>& GetOrCreateStorage()
        {
            std::type_index type = typeid(T);

            auto it = m_Storages.find(type);
            if (it == m_Storages.end())
            {
                Storage storage;
                storage.map = std::unordered_map<EntityHandle, T>{};

                storage.removeFn = [this, type](EntityHandle handle)
                    {
                        auto& map = std::any_cast<std::unordered_map<EntityHandle, T>&>(
                            m_Storages.at(type).map
                        );
                        map.erase(handle);
                    };

                m_Storages[type] = std::move(storage);
                it = m_Storages.find(type);
            }

            return std::any_cast<std::unordered_map<EntityHandle, T>&>(it->second.map);
        }

        template<typename T>
        std::unordered_map<EntityHandle, T>& GetStorage()
        {
            std::type_index type = typeid(T);
            auto it = m_Storages.find(type);
            assert(it != m_Storages.end() && "Storage for this component type doesn't exist");
            return std::any_cast<std::unordered_map<EntityHandle, T>&>(it->second.map);
        }

        template<typename T>
        const std::unordered_map<EntityHandle, T>& GetStorage() const
        {
            std::type_index type = typeid(T);
            auto it = m_Storages.find(type);
            assert(it != m_Storages.end() && "Storage for this component type doesn't exist");
            return std::any_cast<const std::unordered_map<EntityHandle, T>&>(it->second.map);
        }
    };
}