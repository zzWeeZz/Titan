#include "Scene.h"

namespace Titan
{
	void Scene::OnStart()
	{
	}

	void Scene::OnUpdate()
	{
	}

	void Scene::OnRender()
	{
	}

	Entity Scene::CreateEntity()
	{
		return {m_Registry.CreateEntity(), this};
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.DestroyEntity(entity.m_EntityId);
	}
}
