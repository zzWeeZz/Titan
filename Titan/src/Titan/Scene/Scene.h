#pragma once
#include "Snowflake.hpp"
#include "Titan/Rendering/SceneRenderer.h"
namespace Titan
{
	class Scene
	{
		friend class Entity;
		friend class SceneHierarchyPanel;
	public:
		Scene();
		void OnEditorStart();
		void OnEditorUpdate();
		void OnEditorRender();

		Entity CreateEntity();
		void DestroyEntity(Entity entity);

	private:
		Snowflake::Registry m_Registry;
		SceneRenderer m_SceneRenderer;
	};
}
