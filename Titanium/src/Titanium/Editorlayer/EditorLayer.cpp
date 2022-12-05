#include "EditorLayer.h"

#include <ImGui/imgui.h>
#include <ImGui/backends/imgui_impl_vulkan.h>
#include <glm/glm/gtx/euler_angles.hpp>

#include "Titan/Utils/Input.h"
#include "Titan/Utils/Chrono.h"

#include "Titan/Scene/Components.h"

#include "Titan/ImGui/TitanImGui.h"

#include "Titan/Rendering/Renderer.h"
#include "Titan/Rendering/Libraries/SamplerLibrary.h"

#include <Titan/Events/InputEvent.h>

#include "Titan/Assets/Model/Model.h"
#include "Titan/Assets/Texture/Texture.h"
#include "Titan/Assets/Camera/Camera.h"

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
			ResourceRegistry::GetItem<ModelHandle>(mdl.modelHandle)->Initialize("Assets/Models/rat.glb");
			ResourceRegistry::GetItem<Texture>(mdl.textureHandle)->Initialize("Assets/Texture/Titan.jpg");
		}
		{
			auto entity = m_ActiveScene->CreateEntity();
			auto& light = entity.AddComponent<LightComponent>();
		}
		m_transformData.position.z = -5.f;
	}

	void EditorLayer::OnUpdate()
	{
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
	/*	ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + 0));
		ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, 50));
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGuiWindowFlags window_flags = 0
			| ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoScrollbar
			| ImGuiWindowFlags_NoSavedSettings
			;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		ImGui::Begin("TOOLBAR", NULL, window_flags);
		ImGui::PopStyleVar();

		ImGui::Button("Toolbar goes here", ImVec2(0, 37));

		ImGui::End();*/

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
			m_transformData.position -= m_CameraFront * Chrono::Timestep() * 10.f;
		}
		if (Input::Key(Key::W, InputMode::Down))
		{
			m_transformData.position += m_CameraFront * Chrono::Timestep() * 10.f;
		}
		if (Input::Key(Key::D, InputMode::Down))
		{
			m_transformData.position += glm::cross(m_CameraFront, glm::vec3(0,1,0)) * Chrono::Timestep() * 10.f;
		}
		if (Input::Key(Key::A, InputMode::Down))
		{
			m_transformData.position -= glm::cross(m_CameraFront, glm::vec3(0, 1, 0)) * Chrono::Timestep() * 10.f;
		}
		if (Input::Key(Key::E, InputMode::Down))
		{
			m_transformData.position.y -= Chrono::Timestep() * 10.f;
		}
		if (Input::Key(Key::Q, InputMode::Down))
		{
			m_transformData.position.y += Chrono::Timestep() * 10.f;
		}

		m_cameraData.View = glm::lookAt(m_transformData.position, m_transformData.position + m_CameraFront, glm::vec3(0, 1, 0));
		m_cameraData.Projection = glm::perspective(glm::radians(m_cameraData.FOV), m_cameraData.AspectRatio, 0.1f, 200.f);
		CameraCmd cmd{};
		cmd.view = m_cameraData.View;
		cmd.proj = m_cameraData.Projection;
		Renderer::Submit(cmd);
	}
	bool EditorLayer::MouseMove(MouseMoveEvent& event)
	{
		if (Input::MouseButton(Mouse::ButtonRight, InputMode::Down))
		{
			float xoffset = static_cast<float>(event.GetMouseDelta().first);
			float yoffset = static_cast<float>(event.GetMouseDelta().second);


			const float sensitivity = 0.5f;
			xoffset *= sensitivity;
			yoffset *= sensitivity;

			m_Yaw += xoffset;
			m_Pitch += yoffset;

			if (m_Pitch > 89.0f)
				m_Pitch = 89.0f;
			if (m_Pitch < -89.0f)
				m_Pitch = -89.0f;

			glm::vec3 direction;
			direction.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
			direction.y = sin(glm::radians(m_Pitch));
			direction.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
			m_CameraFront = glm::normalize(direction);

			
		}
	
		return false;
	}
}
