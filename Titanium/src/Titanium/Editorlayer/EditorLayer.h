#pragma once
#include "Titan/Layer.h"
#include "Titanium/Panels/SceneHierarchyPanel.h"
#include "Titanium/Panels/PropertiesPanel.h"

#include "Titan/Assets/Camera/Camera.h"
#include "Titan/Scene/Components.h"

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
		void RunEditorCamera();
		CameraComponent m_cameraData;
		TransformComponent m_transformData;
		Ref<Scene> m_ActiveScene;
		Ref<PropertiesPanel> m_PropertiesPanel;
		Ref<SceneHierarchyPanel> m_SceneHierarchyPanel;
	};
}

