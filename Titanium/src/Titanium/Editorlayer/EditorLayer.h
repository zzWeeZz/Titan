#pragma once
#include "Titan/Layer.h"
#include "Titanium/Panels/SceneHierarchyPanel.h"
#include "Titanium/Panels/PropertiesPanel.h"

#include "Titan/Assets/Camera/Camera.h"
#include "Titan/Scene/Components.h"
#include <Titan/Events/InputEvent.h>
#include <Titanium/Panels/PanelHandler.h>
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
		bool MouseMove(MouseMoveEvent& event);
		float m_Pitch;
		float m_Yaw;
		glm::vec3 m_CameraFront;
		CameraComponent m_cameraData;
		TransformComponent m_transformData;
		Ref<Scene> m_ActiveScene;
		PanelHandler m_PanelHandler;

		Ref<PropertiesPanel> m_PropertiesPanel;
		Ref<SceneHierarchyPanel> m_SceneHierarchyPanel;
	};
}

