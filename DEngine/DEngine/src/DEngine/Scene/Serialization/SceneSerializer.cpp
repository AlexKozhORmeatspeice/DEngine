#include "dpch.h"
#include "SceneSerializer.h"

#include "DEngine/Scene/Entity.h"
#include "DEngine/Scene/Components.h"

#include "fstream"
#include "yaml-cpp/yaml.h"

namespace YAML {

	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

	template<>
	struct convert<DEngine::UUID>
	{
		static Node encode(const DEngine::UUID& uuid)
		{
			Node node;
			node.push_back((uint64_t)uuid);
			return node;
		}

		static bool decode(const Node& node, DEngine::UUID& uuid)
		{
			uuid = node.as<uint64_t>();
			return true;
		}
	};

}

namespace DEngine
{
	const std::string TAG_COMP_NAME = "Tag_Component";
	const std::string TAG_COMP_TAG_KEY = "Tag";

	const std::string TRANS_COMP_NAME = "Transform_Component";
	const std::string TRANS_COMP_POS = "Pos";
	const std::string TRANS_COMP_ROT = "Rot";
	const std::string TRANS_COMP_SCALE = "Scale";
	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		: m_Scene(scene)
	{

	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}
	
	static void SerializeEntity(YAML::Emitter& out, const Entity& entity)
	{
		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << "12391231"; //TODO: add entity id

		if (entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << TAG_COMP_NAME;
			out << YAML::BeginMap;

			auto& tagComp = entity.GetComponent<TagComponent>();

			out << YAML::Key << TAG_COMP_TAG_KEY << YAML::Value << tagComp.Tag.c_str();

			out << YAML::EndMap;
		}

		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << TRANS_COMP_NAME;
			out << YAML::BeginMap;

			auto& transComp = entity.GetComponent<TransformComponent>();

			glm::vec3 pos = transComp.GetPosition();
			glm::vec3 rot = transComp.GetRotationEuler();
			glm::vec3 scale = transComp.GetScale();

			out << YAML::Key << TRANS_COMP_POS << YAML::Value << pos;
			out << YAML::Key << TRANS_COMP_ROT << YAML::Value << rot;
			out << YAML::Key << TRANS_COMP_SCALE << YAML::Value << scale;

			out << YAML::EndMap;
		}

		out << YAML::EndMap;
	}

	void SceneSerializer::Serialize(const std::string& filepath)
	{
		if (m_Scene == nullptr)
		{
			D_CORE_ERROR("Try to serialize object with null scene obj");
			return;
		}

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		for (auto& entity : m_Scene->GetAllEntities())
		{
			if (!entity)
				return;

			SerializeEntity(out, entity);
		}
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	void SceneSerializer::SerializeRuntime(const std::string& filepath)
	{
		//WIP
		D_CORE_ASSERT(false, "");
	}

	bool SceneSerializer::Deserialize(const std::string& filepath)
	{
		std::ifstream fin(filepath);
		std::stringstream ss;
		ss << fin.rdbuf();

		YAML::Node data = YAML::Load(ss.str());
		if (!data["Scene"])
			return false;

		std::string	sceneName = data["Scene"].as<std::string>();
		D_CORE_INFO("Deserialize scene {0}", sceneName);

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				uint64_t uuid = entity["Entity"].as<uint64_t>();

				std::string name;
				auto tagCompText = entity[TAG_COMP_NAME];
				if (tagCompText)
				{
					name = tagCompText[TAG_COMP_TAG_KEY].as<std::string>();
				}

				D_CORE_INFO("Deserialize entity with ID: {0}, Name: {1}", uuid, name);

				Entity& entityObj = m_Scene->CreateEntity(name);
				auto transCompText = entity[TRANS_COMP_NAME];
				if (transCompText)
				{
					auto& transComp = entityObj.GetComponent<TransformComponent>();
					transComp.SetPosition(transCompText[TRANS_COMP_POS].as<glm::vec3>());
					transComp.SetRotationEuler(transCompText[TRANS_COMP_ROT].as<glm::vec3>());
					transComp.SetScale(transCompText[TRANS_COMP_SCALE].as<glm::vec3>());
				}
			}
		}

		return true;
	}

	bool SceneSerializer::DeserializeRuntime(const std::string& filepath)
	{
		//WIP
		D_CORE_ASSERT(false, "");
		return false;
	}
}