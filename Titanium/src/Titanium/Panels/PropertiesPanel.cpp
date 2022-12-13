#include "PropertiesPanel.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "Titan/Scene/Components.h"
#include "Titanium/Utils/ImGuiUtils.h"
#include "ImGui/misc/cpp/imgui_stdlib.h"
namespace Titan
{
	void PropertiesPanel::Inspect(Entity entity)
	{
		m_InspectedItem = entity;
	}
	void PropertiesPanel::OnImGuiDraw()
	{
		ImGui::Begin("Properties");
		if (!m_InspectedItem())
		{
			ImGui::Text("Nothing is selected.");
			ImGui::End();
			return;
		}

		{
			auto& tagComp = m_InspectedItem.GetComponent<TagComponent>();
			ImGuiUtils::String("Tag:", tagComp.tag);
		}

		{
			auto& transComp = m_InspectedItem.GetComponent<TransformComponent>();
			ImGuiUtils::Vec3("Position:", transComp.position);
			glm::vec3 degRot = glm::degrees(transComp.rotation);
			ImGuiUtils::Vec3("Rotation:", degRot);
			transComp.rotation = glm::radians(degRot);
			ImGuiUtils::Vec3("Scale:", transComp.scale);
		}

		if(m_InspectedItem.HasComponent<LightComponent>())
		{
			auto& lightComp = m_InspectedItem.GetComponent<LightComponent>();
			ImGuiUtils::Color3("Color:", lightComp.color);
			ImGuiUtils::Float("Intensity:", lightComp.intensity);
		}
		ImGui::End();
	}
}