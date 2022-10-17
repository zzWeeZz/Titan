#include "EditorLayer.h"

#include "Titan/Assets/Camera/Camera.h"
#include "Titan/Scene/Components.h"
#include "Titan/Utils/Input.h"
#include "Titan/Assets/Model/Model.h"

namespace Titan
{
	void EditorLayer::OnAttach()
	{
		m_ActiveScene = CreateRef<Scene>();
		m_SceneHierarchyPanel = std::make_shared<SceneHierarchyPanel>(m_ActiveScene);
		{
			auto entity = m_ActiveScene->CreateEntity();
			auto& mdl = entity.AddComponent<ModelComponent>();
			ResourceRegistry::GetItem<ModelHandle>(mdl.modelHandle)->Initialize("Assets/Models/Cube.gltf");
		}
		{
			auto entity = m_ActiveScene->CreateEntity();
			auto& mdl = entity.AddComponent<ModelComponent>();
			ResourceRegistry::GetItem<ModelHandle>(mdl.modelHandle)->Initialize("Assets/Models/box01.glb");
			entity.GetComponent<TransformComponent>().position.x = 10;
		}
		m_transformData.position.z = -5.f;
	}

	void EditorLayer::OnUpdate()
	{
		m_ActiveScene->OnEditorUpdate();
		RunEditorCamera();
	}

	void EditorLayer::OnDetach()
	{

	}

	void EditorLayer::OnEvent(Event& e)
	{

	}
	void EditorLayer::RunEditorCamera()
	{
		if (Input::Key(Key::S, InputMode::Down))
		{
			m_transformData.position.z -= 0.005f;
		}
		if (Input::Key(Key::W, InputMode::Down))
		{
			m_transformData.position.z += 0.005f;
		}
		if (Input::Key(Key::D, InputMode::Down))
		{
			m_transformData.position.x -= 0.005f;
		}
		if (Input::Key(Key::A, InputMode::Down))
		{
			m_transformData.position.x += 0.005f;
		}


		CameraSystem(m_cameraData, m_transformData);
	}
}
