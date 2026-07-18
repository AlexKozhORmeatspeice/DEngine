#pragma once

#include "DEngine/Core.h"
#include "DEngine/Asset/Asset.h"

#include "DEngine/Scene/Component.h"

namespace DEngine
{
	struct MeshRendererComponent : public Component
	{
		AssetHandle meshHandle;
		AssetHandle materialHandle;

		MeshRendererComponent() = default;
		MeshRendererComponent(const MeshRendererComponent&) = default;
		MeshRendererComponent(const AssetHandle& _mesh, const AssetHandle& _mat)
			: meshHandle(_mesh), materialHandle(_mat) { }

		//TODO: доделать сериализацию
		virtual void Serialize(YAML::Emitter& out) const override {}
		virtual bool Deserialize(const YAML::Node& node) override { return true; }

		DECLARE_COMPONENT(MeshRendererComponent);
	};
}
