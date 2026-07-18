#pragma once

#include <tuple>
#include <string>
#include <yaml-cpp/yaml.h>
#include "DEngine/Scene/Scene.h"
#include "DEngine/Scene/Entity.h"
#include "DEngine/Scene/Components.h"

#include "filesystem"

namespace DEngine
{
    //Сюда добавляем новые компоненты которые нужно сериализовать
    using ComponentList = std::tuple<TagComponent, TransformComponent, MeshRendererComponent, DirectLightComponent>;

    struct DeserializeSceneResult
    {
        bool isSuccessful;
        Ref<Scene> scene;
    };

    class SceneSerializer
    {
    public:
        static void Serialize(const Ref<Scene>& scene, const std::filesystem::path& path);
        static void SerializeRuntime(const Ref<Scene>& scene, const std::filesystem::path& path);
        
        static DeserializeSceneResult Deserialize(const std::filesystem::path& path);
        static DeserializeSceneResult DeserializeRuntime(const std::filesystem::path& path);

    private:
        template<typename T>
        static void SerializeComponent(YAML::Emitter& out, const Entity& entity);

        template<typename T>
        static bool DeserializeComponent(const YAML::Node& entityNode, Entity& entity);

        template<typename Tuple>
        static void SerializeComponents(YAML::Emitter& out, const Entity& entity);

        template<typename Tuple>
        static bool DeserializeComponents(const YAML::Node& entityNode, Entity& entity);

        static void SerializeEntity(YAML::Emitter& out, const Entity& entity);
        static bool DeserializeEntity(const YAML::Node& entityNode, Entity& entity);
    };
}