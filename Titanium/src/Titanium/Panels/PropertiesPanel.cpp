#include "PropertiesPanel.h"
#include "ImGui/imgui.h"
#include "Titan/Scene/Components.h"
#include "ImGui/misc/cpp/imgui_stdlib.h"
namespace Titan
{
	void PropertiesPanel::Inspect(Entity entity)
	{
		m_InspectedItem = entity;
	}
	void PropertiesPanel::ImGuiBeginRender()
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
			ImGui::Text("Tag: ");
			ImGui::SameLine();
			std::string input = tagComp.tag.c_str();
			if (ImGui::InputText("##Tag", &input))
			{
				tagComp.tag = input;
			}
		}


		ImGui::End();
	}
}