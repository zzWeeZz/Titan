#include "Entity.h"

namespace Titan
{
	Entity::Entity(Snowflake::Entity entity, Scene* scene)
	{
		m_EntityId = entity;
		m_Scene = scene;
	}
}
