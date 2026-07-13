#include "Registry.h"

namespace DaddyEngine
{
	uint32_t Registry::CreateEntity()
	{
		return m_NextEntity++;
	}

	void Registry::DestroyEntity(uint32_t entity)
	{
		for (auto& pair : m_ComponentPools)
		{
			pair.second->EntityDestroyed(entity);
		}
	}
}