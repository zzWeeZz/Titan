#pragma once
#include "Titan/Scene/Scene.h"
#include "Titan/Scene/Entity.h"
#include "Titan/Core/Core.h"
#include "Titanium/Panels/Panel.h"
namespace Titan
{
	class SceneHierarchyPanel : public Panel
	{
	public:
		SceneHierarchyPanel(const Ref<Scene>& context);

		void SetContext(const Ref<Scene>& context);
		Entity GetSelectedEntity() const { return m_SelectedEntity; }
		// Inherited via Panel
		virtual void OnImGuiDraw() override;
		void EntitySelectedCallback(std::function<void(Entity& selectedEntity)>&& callback);
	private:
		void DrawEntityNode(Entity entity);

		Ref<Scene> m_Context;
		Entity m_SelectedEntity;

		
	};
}
