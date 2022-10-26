#pragma once
#include "Titan/Scene/Scene.h"
#include "Titan/Scene/Entity.h"
#include "Titan/Core/Core.h"
namespace Titan
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel(const Ref<Scene>& context);

		void SetContext(const Ref<Scene>& context);
		Entity GetSelectedEntity() const { return m_SelectedEntity; }
		void OnImGuiRender();
	private:
		void DrawEntityNode(Entity entity);

		Ref<Scene> m_Context;
		Entity m_SelectedEntity;
	};
}
