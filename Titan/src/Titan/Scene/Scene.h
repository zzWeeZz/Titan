#pragma once
#include "Entity.h"

namespace Titan
{
	class Scene
	{
		friend class Entity;
	public:
		void OnStart();
		void OnUpdate();
		void OnRender();

		Entity CreateEntity();
		void DestroyEntity(Entity entity);

	private:
		Snowflake::Registry m_Registry;
	};
}
