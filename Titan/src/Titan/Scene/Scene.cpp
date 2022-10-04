#include "Scene.h"
#include "Entity.h"
#include "Components.h"
#include "Titan/Assets/Camera/Camera.h"
#include "Titan/Rendering/Renderer.h"

namespace Titan
{
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
				/*if (!mdl.modelHandle)
				{
					mdl.modelHandle = Model::Create(mdl.filePath);
				}
				mdl.modelHandle->SetPosition(tf.position);
				mdl.modelHandle->SetRotation(tf.quaternion);
				mdl.modelHandle->SetScale(tf.scale);
				Renderer::SubmitMesh(mdl.modelHandle);*/
			});
	}

	void Scene::OnEditorRender()
	{
	}

	Entity Scene::CreateEntity()
	{
		Entity ent = { m_Registry.CreateEntity(), this };
		ent.AddComponent<TagComponent>();
		ent.AddComponent<TransformComponent>();
		return ent;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.DestroyEntity(entity.m_EntityId);
	}
}
