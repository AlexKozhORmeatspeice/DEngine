#include "dpch.h"
#include "Entity.h"

namespace DEngine
{
	Entity::Entity()
	{

	}

	Entity::Entity(entt::entity _handle, Scene* scene)	
		: m_Handle(_handle), m_Scene(scene)
	{

	}

	Entity::~Entity()
	{

	}
}