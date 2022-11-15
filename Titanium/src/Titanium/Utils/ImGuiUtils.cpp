#include "ImGuiUtils.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/misc/cpp/imgui_stdlib.h"

#include "Snowflake/String.hpp"

void Titan::ImGuiUtils::Vec3(const std::string& label, glm::vec3& vec)
{
	ImGui::PushID(label.c_str());
	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, 100.f);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();
	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImVec2 buttonsize = { lineHeight + 3.0f, lineHeight };
	if (ImGui::Button("X", buttonsize))
	{
		vec.x = 0;
	}
	ImGui::SameLine();
	ImGui::DragFloat("##X", &vec.x, 0.1f);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::Button("Y", buttonsize))
	{
		vec.y = 0;
	}
	ImGui::SameLine();
	ImGui::DragFloat("##Y", &vec.y, 0.1f);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::Button("Z", buttonsize))
	{
		vec.z = 0;
	}
	ImGui::SameLine();
	ImGui::DragFloat("##Z", &vec.z, 0.1f);
	ImGui::PopItemWidth();
	ImGui::PopStyleVar();
	ImGui::Columns(1);
	ImGui::PopID();
}

void Titan::ImGuiUtils::Quat(const std::string& label, glm::quat& quat)
{

	ImGui::PushID(label.c_str());
	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, 100.f);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();
	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImVec2 buttonsize = { lineHeight + 3.0f, lineHeight };
	glm::vec3 vec = glm::eulerAngles(quat);
	if (ImGui::Button("X", buttonsize))
	{
		vec.x = 0;
	}
	ImGui::SameLine();
	ImGui::DragFloat("##X", &vec.x, 0.1f);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::Button("Y", buttonsize))
	{
		vec.y = 0;
	}
	ImGui::SameLine();
	ImGui::DragFloat("##Y", &vec.y, 0.1f);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::Button("Z", buttonsize))
	{
		vec.z = 0;
	}
	ImGui::SameLine();
	ImGui::DragFloat("##Z", &vec.z, 0.1f);
	ImGui::PopItemWidth();
	ImGui::PopStyleVar();
	ImGui::Columns(1);
	quat = glm::tquat<float>(vec);
	ImGui::PopID();
}

void Titan::ImGuiUtils::String(const std::string& label, std::string& str)
{
	ImGui::PushID(label.c_str());
	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, 100.f);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();
	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

	ImGui::SameLine();
	std::string bufferText = str;
	if (ImGui::InputText("##Text", &bufferText))
	{
		str = bufferText;
	};
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();
	ImGui::Columns(1);
	ImGui::PopID();
}

void Titan::ImGuiUtils::String(const std::string& label, Snowflake::String& str)
{
	ImGui::PushID(label.c_str());
	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, 100.f);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();
	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

	ImGui::SameLine();
	std::string bufferText = str.c_str();
	if (ImGui::InputText("##Text", &bufferText))
	{
		str = bufferText;
	};
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();
	ImGui::Columns(1);
	ImGui::PopID();
}
