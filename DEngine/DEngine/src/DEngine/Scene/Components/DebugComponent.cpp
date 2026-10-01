#include "dpch.h"
#include "DebugComponent.h"

namespace DEngine
{

	void DebugComponent::Serialize(YAML::Emitter& out) const
	{
		
	}

	bool DebugComponent::Deserialize(const YAML::Node& node)
	{
		return true;
	}
}