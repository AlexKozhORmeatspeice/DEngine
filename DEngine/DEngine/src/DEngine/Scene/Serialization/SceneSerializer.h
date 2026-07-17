#pragma once
#include "DEngine/Core.h"
#include "DEngine/Scene/Scene.h"

namespace DEngine
{
	class SceneSerializer
	{
	public:
		SceneSerializer() = default;
		SceneSerializer(const Ref<Scene>& scene);
		
		void Serialize(const std::string& filepath);
		void SerializeRuntime(const std::string& filepath);

		bool Deserialize(const std::string& filepath);
		bool DeserializeRuntime(const std::string& filepath);
	private:
		Ref<Scene> m_Scene;
	};
}

