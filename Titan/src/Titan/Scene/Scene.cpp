#include "TNpch.h"
#include "Scene.h"
#include "Entity.h"
#include "Components.h"
#include "Titan/Assets/Camera/Camera.h"
#include "Titan/Assets/Model/Model.h"
#include "Titan/Rendering/Renderer.h"

namespace Titan
{
	Scene::Scene() : m_SceneRenderer(m_Registry)
	{
	}

	void Scene::OnEditorStart()
	{
	}

	void Scene::OnEditorUpdate()
	{
		m_Registry.Execute<CameraComponent, TransformComponent>([&](auto& entity, CameraComponent& C, TransformComponent& tf)
			{
				CameraSystem(C, tf);
			});
		m_Registry.Execute<ModelComponent, TransformComponent>([&](auto& entity, ModelComponent& mdl, TransformComponent& tf)
			{
				ModelSystem(mdl, tf);
			});
	}

	void Scene::OnEditorRender()
	{
		m_SceneRenderer.Render();
	}

	Entity Scene::CreateEntity()
	{
		Entity ent = { m_Registry.CreateEntity(), this };
		ent.AddComponent<TagComponent>().tag = "new Entity";
		ent.AddComponent<TransformComponent>();
		return ent;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.DestroyEntity(entity.m_EntityId);
	}
}
