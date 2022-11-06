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
			ImGuiUtils::Vec3("Pos:", transComp.position);
			ImGuiUtils::Quat("Rot:", transComp.quaternion);
			ImGuiUtils::Vec3("Scale:", transComp.scale);
		}
		ImGui::End();
	}
}