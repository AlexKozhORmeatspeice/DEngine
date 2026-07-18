#include "dpch.h"
#include "SceneSerializer.h"

#include "DEngine/Scene/Entity.h"
#include "DEngine/Scene/Components.h"

#include "fstream"
#include "yaml-cpp/yaml.h"

namespace DEngine
{
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

    void SceneSerializer::Serialize(const Ref<Scene>& scene, const std::string& filepath)
    {
        if (scene == nullptr)
        {
            D_CORE_ERROR("Try to serialize null scene object");
            return;
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

        std::ofstream fout(filepath);
        if (!fout.is_open())
        {
            D_CORE_ERROR("Failed to open file for writing: {0}", filepath);
            return;
        }

        fout << out.c_str();
        fout.close();
    }

    void SceneSerializer::SerializeRuntime(const Ref<Scene>& scene, const std::string& filepath)
    {
        D_CORE_ASSERT(false, "SceneSerializer::SerializeRuntime: No implemented logic for runtime scene serialization");
    }

    DeserializeSceneResult SceneSerializer::Deserialize(const std::string& filepath)
    {
        DeserializeSceneResult result;
        result.isSuccessful = false;
        result.scene = nullptr;

        std::ifstream fin(filepath);
        if (!fin.is_open())
        {
            D_CORE_ERROR("Failed to open file for reading: {0}", filepath);
            return result;
        }

        std::stringstream ss;
        ss << fin.rdbuf();
        fin.close();

        YAML::Node data = YAML::Load(ss.str());
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
            for (auto entityNode : entitiesNode)
            {
                Entity entity = newScene->CreateEntity();

                if (!DeserializeEntity(entityNode, entity))
                {
                    D_CORE_WARN("Failed to deserialize entity, skipping...");
                    continue;
                }

                D_CORE_INFO("Deserialized entity with ID: {0}", entity.GetUUID());
            }
        }

        result.isSuccessful = true;
        result.scene = newScene;
        return result;
    }

    DeserializeSceneResult SceneSerializer::DeserializeRuntime(const std::string& filepath)
    {
        D_CORE_ASSERT(false, "SceneSerializer::DeserializeRuntime: No implemented logic for runtime scene deserialization");
        
        DeserializeSceneResult result;
        result.isSuccessful = false;
        result.scene = nullptr;
        return result;
    }
}