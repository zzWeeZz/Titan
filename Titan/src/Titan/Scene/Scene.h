#pragma once
#include "Snowflake.hpp"

namespace Titan
{
	class Scene
	{
		friend class Entity;
		friend class SceneHierarchyPanel;
	public:
		Scene() = default;
		void OnEditorStart();
		void OnEditorUpdate();
		void OnEditorRender();

		Entity CreateEntity();
		void DestroyEntity(Entity entity);

	private:
		Snowflake::Registry m_Registry;
	};
}
