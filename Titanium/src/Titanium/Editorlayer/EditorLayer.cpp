#include "EditorLayer.h"

#include "Titan/Assets/Camera/Camera.h"
#include "Titan/Scene/Components.h"
#include "Titan/Utils/Input.h"
#include "Titan/Assets/Model/Model.h"
#include "Titan/Assets/Texture/Texture.h"
#include <imgui.h>
#include <Titan/Events/InputEvent.h>
#include <backends/imgui_impl_vulkan.h>
#include "Titan/Rendering/Renderer.h"
#include "Titan/Rendering/Libraries/SamplerLibrary.h"
#include "Titan/ImGui/TitanImGui.h"
namespace Titan
{
	void EditorLayer::OnAttach()
	{
		m_ActiveScene = CreateRef<Scene>();
		m_PanelHandler.Add<SceneHierarchyPanel>("SceneHierarchy", std::make_shared<SceneHierarchyPanel>(m_ActiveScene));
		m_PanelHandler.Add<PropertiesPanel>("PropertiesPanel", std::make_shared<PropertiesPanel>());
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
		m_PanelHandler.SendCallback();
		m_PanelHandler.Get<SceneHierarchyPanel>("SceneHierarchy")->EntitySelectedCallback([this](Entity& entity)
			{
				m_PanelHandler.Get<PropertiesPanel>("PropertiesPanel")->Inspect(entity);
			});

		ImGui::Begin("ViewPort");
		if (ImGui::GetContentRegionAvail().x != Renderer::GetMainFramebuffer()->GetInfo().width
			|| ImGui::GetContentRegionAvail().y != Renderer::GetMainFramebuffer()->GetInfo().height)
		{
			Renderer::GetMainFramebuffer()->Resize(static_cast<size_t>(ImGui::GetContentRegionAvail().x), static_cast<size_t>(ImGui::GetContentRegionAvail().y));
			m_cameraData.AspectRatio = ImGui::GetContentRegionAvail().x / ImGui::GetContentRegionAvail().y;
		}

		ImGui::Image(Renderer::GetMainFramebuffer()->GetDescriptorSet(), ImGui::GetContentRegionAvail());
		ImGui::End();
		m_ActiveScene->OnEditorUpdate();

		RunEditorCamera();
	}

	void EditorLayer::OnDetach()
	{

	}

	void EditorLayer::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseMoveEvent>(TN_BIND_FUNC(EditorLayer::MouseMove));
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
	bool EditorLayer::MouseMove(MouseMoveEvent& event)
	{

		return false;
	}
}
