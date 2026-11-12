#include "dpch.h"
#include "SceneSerializer.h"

#include "DEngine/Scene/Entity.h"
#include "DEngine/Scene/Components.h"

#include "DEngine/Jobs/JobSystem.h"

#include "fstream"
#include "sstream"
#include "optional"
#include "yaml-cpp/yaml.h"

namespace DEngine
{
    template<typename... T>
    struct ParsedEntity
    {
        uint64_t UUID = 0;
        bool HasUUID = false;
        bool Ok = true;
        std::tuple<std::optional<T>...> Components;
    };

    template<typename T>
    void ReadParsedSlot(const YAML::Node& node, std::optional<T>& slot, bool& ok)
    {
        const YAML::Node child = node[T::GetStaticTypeName()];
        if (!child)
            return;

        T value;
        if (!value.Deserialize(child))
        {
            ok = false;
            return;
        }
        slot = std::move(value);
    }

    template<typename T>
    void ApplyParsedSlot(Entity& entity, std::optional<T>& slot)
    {
        if (!slot)
            return;
        entity.AddComponent<T>() = std::move(*slot);
    }

    template<typename... T>
    ParsedEntity<T...> ParseEntityTextImpl(const std::string& text)
    {
        ParsedEntity<T...> parsed;
        try
        {
            YAML::Node node = YAML::Load(text);
            if (node["Entity"])
            {
                parsed.UUID = node["Entity"].as<uint64_t>();
                parsed.HasUUID = true;
            }

            std::apply([&](auto&... slot)
            {
                (ReadParsedSlot(node, slot, parsed.Ok), ...);
            }, parsed.Components);
        }
        catch (const std::exception&)
        {
            parsed.Ok = false;
        }
        return parsed;
    }

    template<typename Tuple>
    struct ParsedEntityOf;

    template<typename... T>
    struct ParsedEntityOf<std::tuple<T...>>
    {
        using Type = ParsedEntity<T...>;
    };

    using ParsedSceneEntity = ParsedEntityOf<ComponentList>::Type;

    template<typename... T>
    ParsedSceneEntity ParseFromList(const std::string& text, std::tuple<T...>*)
    {
        return ParseEntityTextImpl<T...>(text);
    }

    ParsedSceneEntity ParseEntityText(const std::string& text)
    {
        return ParseFromList(text, static_cast<ComponentList*>(nullptr));
    }

    template<typename T>
    void SceneSerializer::SerializeComponent(YAML::Emitter& out, const Entity& entity)
    {
        if (entity.HasComponent<T>())
        {
            auto& comp = entity.GetComponent<T>();
            out << YAML::Key << comp.GetTypeName();
            out << YAML::BeginMap;
            comp.Serialize(out);
            out << YAML::EndMap;
        }
    }

    template<typename T>
    bool SceneSerializer::DeserializeComponent(const YAML::Node& entityNode, Entity& entity)
    {
        std::string typeName = T::GetStaticTypeName();
        if (entityNode[typeName])
        {
            auto& comp = entity.AddComponent<T>();
            return comp.Deserialize(entityNode[typeName]);
        }
        return true;
    }

    template<typename Tuple>
    void SceneSerializer::SerializeComponents(YAML::Emitter& out, const Entity& entity)
    {
        std::apply([&](auto... types) {
            (SerializeComponent<decltype(types)>(out, entity), ...);
        }, Tuple{});
    }

    template<typename Tuple>
    bool SceneSerializer::DeserializeComponents(const YAML::Node& entityNode, Entity& entity)
    {
        return std::apply([&](auto... types) {
            return (DeserializeComponent<decltype(types)>(entityNode, entity) && ...);
        }, Tuple{});
    }

    void SceneSerializer::SerializeEntity(YAML::Emitter& out, const Entity& entity)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

        SerializeComponents<ComponentList>(out, entity);

        out << YAML::EndMap;
    }

    bool SceneSerializer::DeserializeEntity(const YAML::Node& entityNode, Entity& entity)
    {
        if (entityNode["Entity"])
        {
            uint64_t uuid = entityNode["Entity"].as<uint64_t>();
            entity.SetUUID(uuid);
        }

        return DeserializeComponents<ComponentList>(entityNode, entity);
    }

    std::string SceneSerializer::SerializeToString(const Ref<Scene>& scene)
    {
        if (scene == nullptr)
        {
            D_CORE_ERROR("Try to serialize null scene object");
            return {};
        }

        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Scene" << YAML::Value << "Untitled";
        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

        for (auto& entity : scene->GetAllEntities())
        {
            if (!entity)
                continue;

            SerializeEntity(out, entity);
        }

        out << YAML::EndSeq;
        out << YAML::EndMap;

        return out.c_str();
    }

    void SceneSerializer::Serialize(const Ref<Scene>& scene, const std::filesystem::path& path)
    {
        const std::string yaml = SerializeToString(scene);
        if (yaml.empty())
            return;

        std::ofstream fout(path);
        if (!fout.is_open())
        {
            D_CORE_ERROR("Failed to open file for writing: {0}", path.string());
            return;
        }

        fout << yaml;
        fout.close();
    }

    void SceneSerializer::SerializeRuntime(const Ref<Scene>& scene, const std::filesystem::path& path)
    {
        D_CORE_ASSERT(false, "SceneSerializer::SerializeRuntime: No implemented logic for runtime scene serialization");
    }

    DeserializeSceneResult SceneSerializer::Deserialize(const std::filesystem::path& path)
    {
        DeserializeSceneResult result;
        result.isSuccessful = false;
        result.scene = nullptr;

        std::ifstream fin(path);
        if (!fin.is_open())
        {
            D_CORE_ERROR("Failed to open file for reading: {0}", path.string());
            return result;
        }

        std::stringstream ss;
        ss << fin.rdbuf();
        fin.close();

        return DeserializeFromString(ss.str());
    }

    DeserializeSceneResult SceneSerializer::DeserializeFromString(const std::string& yaml)
    {
        DeserializeSceneResult result;
        result.isSuccessful = false;
        result.scene = nullptr;

        if (yaml.empty())
        {
            D_CORE_ERROR("Scene YAML is empty");
            return result;
        }

        YAML::Node data = YAML::Load(yaml);
        if (!data["Scene"])
        {
            D_CORE_ERROR("Invalid scene file: missing 'Scene' root node");
            return result;
        }

        std::string sceneName = data["Scene"].as<std::string>();
        D_CORE_INFO("Deserializing scene: {0}", sceneName);

        Ref<Scene> newScene = CreateRef<Scene>();

        auto entitiesNode = data["Entities"];
        if (entitiesNode)
        {
            // yaml-cpp nodes share memory, so each entity is copied to its own
            // string before any worker reads it.
            std::vector<std::string> texts;
            for (auto entityNode : entitiesNode)
                texts.push_back(YAML::Dump(entityNode));

            std::vector<ParsedSceneEntity> parsed(texts.size());
            if (!texts.empty() && JobSystem::IsInitialized())
            {
                Job parse(static_cast<uint32_t>(texts.size()), [&](const JobRange& range, uint32_t)
                {
                    for (uint32_t i = range.Start; i < range.End; i++)
                        parsed[i] = ParseEntityText(texts[i]);
                }, 32);
                JobSystem::Schedule(parse);
                JobSystem::Wait(parse);
            }
            else
            {
                for (size_t i = 0; i < texts.size(); i++)
                    parsed[i] = ParseEntityText(texts[i]);
            }

            for (ParsedSceneEntity& entityData : parsed)
            {
                Entity entity = newScene->CreateEmptyEntity();
                if (entityData.HasUUID)
                    entity.SetUUID(entityData.UUID);

                std::apply([&](auto&... slot)
                {
                    (ApplyParsedSlot(entity, slot), ...);
                }, entityData.Components);

                if (!entityData.Ok)
                {
                    D_CORE_WARN("Failed to deserialize entity, skipping...");
                    continue;
                }

                D_CORE_INFO("Deserialized entity with ID: {0}", entity.GetUUID());
            }
        }
        newScene->RebuildActivity();

        result.isSuccessful = true;
        result.scene = newScene;
        return result;
    }

    DeserializeSceneResult SceneSerializer::DeserializeRuntime(const std::filesystem::path& path)
    {
        D_CORE_ASSERT(false, "SceneSerializer::DeserializeRuntime: No implemented logic for runtime scene deserialization");
        
        DeserializeSceneResult result;
        result.isSuccessful = false;
        result.scene = nullptr;
        return result;
    }
}