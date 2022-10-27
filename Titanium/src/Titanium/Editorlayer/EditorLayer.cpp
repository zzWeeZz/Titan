#include "EditorLayer.h"

#include "Titan/Assets/Camera/Camera.h"
#include "Titan/Scene/Components.h"
#include "Titan/Utils/Input.h"
#include "Titan/Assets/Model/Model.h"
#include "Titan/Assets/Texture/Texture.h"
#include <imgui.h>

namespace Titan
{
	void EditorLayer::OnAttach()
	{
		m_ActiveScene = CreateRef<Scene>();
		m_SceneHierarchyPanel = std::make_shared<SceneHierarchyPanel>(m_ActiveScene);
		m_PropertiesPanel = std::make_shared<PropertiesPanel>();
		{
			auto entity = m_ActiveScene->CreateEntity();
			auto& mdl = entity.AddComponent<ModelComponent>();
			ResourceRegistry::GetItem<ModelHandle>(mdl.modelHandle)->Initialize("Assets/Models/Cube.gltf");
			ResourceRegistry::GetItem<Texture>(mdl.textureHandle)->Initialize("Assets/Texture/Titan.png");
		}
		{
			auto entity = m_ActiveScene->CreateEntity();
			auto& mdl = entity.AddComponent<ModelComponent>();
			ResourceRegistry::GetItem<ModelHandle>(mdl.modelHandle)->Initialize("Assets/Models/box01.glb");
			entity.GetComponent<TransformComponent>().position.x = 10;
		}
		{
			auto entity = m_ActiveScene->CreateEntity();
			auto& mdl = entity.AddComponent<ModelComponent>();
			ResourceRegistry::GetItem<ModelHandle>(mdl.modelHandle)->Initialize("Assets/Models/box01.glb");
			entity.GetComponent<TransformComponent>().position.x = -10;
		}
		{
			auto entity = m_ActiveScene->CreateEntity();
			auto& mdl = entity.AddComponent<ModelComponent>();
			ResourceRegistry::GetItem<ModelHandle>(mdl.modelHandle)->Initialize("Assets/Models/box01.glb");
			entity.GetComponent<TransformComponent>().position.y = 10;
		}
		{
			auto entity = m_ActiveScene->CreateEntity();
			auto& mdl = entity.AddComponent<ModelComponent>();
			ResourceRegistry::GetItem<ModelHandle>(mdl.modelHandle)->Initialize("Assets/Models/box01.glb");
			entity.GetComponent<TransformComponent>().position.y = -10;
		}

		m_transformData.position.z = -5.f;
	}

	void EditorLayer::OnUpdate()
	{
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
		m_SceneHierarchyPanel->OnImGuiRender();
		m_SceneHierarchyPanel->EntitySelectedCallback([this](Entity& entity)
			{
				m_PropertiesPanel->Inspect(entity);
			});
		m_PropertiesPanel->ImGuiBeginRender();
		m_ActiveScene->OnEditorUpdate();
		ImGui::Begin("Test");
		ImGui::Text("Titan is saying hi in imgui");
		ImGui::End();
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
