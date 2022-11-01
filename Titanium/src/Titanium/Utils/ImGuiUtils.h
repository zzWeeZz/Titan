#pragma once
#include "glm/glm.hpp"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include <string>
namespace Titan
{
	namespace ImGuiUtils
	{
		inline void DrawVec3(const std::string& label, glm::vec3& vec)
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
		inline void DrawQuat(const std::string& label, glm::quat& quat)
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
	}
}