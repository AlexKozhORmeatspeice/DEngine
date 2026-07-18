#include "dpch.h"
#include "ComponentFactory.h"

namespace DEngine
{
	ComponentFactory& ComponentFactory::Get()
	{
		static ComponentFactory instance;
		return instance;
	}

	void ComponentFactory::RegisterComponent(const std::string& typeName, ComponentCreator creator)
	{
		m_Creators[typeName] = creator;
	}

	std::unique_ptr<Component> ComponentFactory::CreateComponent(const std::string& typeName) const
	{
		auto it = m_Creators.find(typeName);
		if (it != m_Creators.end())
			return it->second();
		return nullptr;
	}

	bool ComponentFactory::IsComponentRegistered(const std::string& typeName) const
	{
		return m_Creators.find(typeName) != m_Creators.end();
	}
}
