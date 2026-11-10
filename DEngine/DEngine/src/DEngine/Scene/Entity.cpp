#include "dpch.h"
#include "Entity.h"

namespace DEngine
{
	void Entity::SetUUID(uint64_t uuid)
	{
		if (m_Scene && m_Handle != entt::null)
		{
			auto current = m_Scene->m_UUIDByEntity.find(m_Handle);
			if (current != m_Scene->m_UUIDByEntity.end() && current->second != uuid)
				m_Scene->m_EntityByUUID.erase(current->second);

			if (uuid != 0)
			{
				auto occupied = m_Scene->m_EntityByUUID.find(uuid);
				if (occupied != m_Scene->m_EntityByUUID.end() && occupied->second != m_Handle)
					m_Scene->m_UUIDByEntity.erase(occupied->second);
				m_Scene->m_EntityByUUID[uuid] = m_Handle;
			}

			m_Scene->m_UUIDByEntity[m_Handle] = uuid;
		}

		m_UUID = uuid;
		if (!m_Scene)
			return;

		for (Entity& stored : m_Scene->m_Entities)
		{
			if (stored.m_Handle == m_Handle)
				stored.m_UUID = uuid;
		}
	}
}