#pragma once
#include "Titan/Layer.h"
#include "Titanium/Panels/SceneHierarchyPanel.h"


namespace Titan
{
	class EditorLayer : public Layer
	{
	public:
		void OnAttach() override;
		void OnUpdate() override;
		void OnDetach() override;
		void OnEvent(Event& e) override;
	private:
		Ref<Scene> m_ActiveScene;
		Ref<SceneHierarchyPanel> m_SceneHierarchyPanel;
	};
}

