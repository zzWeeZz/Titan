#include "RenderingStatsPanel.h"

#include <ImGui/imgui.h>

#include "Titan/Utils/Profiler.h"

#include "Titan/Rendering/GraphicsContext.h"

namespace Titan
{
	void RenderingStatsPanel::OnImGuiDraw()
	{
		ImGui::Begin("Rendering stats");

		ImGui::Text("GPU Information");
		ImGui::BeginChild("##GPU Information", ImVec2(0, (ImGui::GetWindowSize().y / 2) - ImGui::GetItemRectSize().y * 2.6f));
		ImGui::Text("GPU: ");
		ImGui::SameLine();
		auto& gpuInfo = GraphicsContext::GetPhysicalDevice().GetGPUInfo();
		ImGui::Text(gpuInfo.gpuName.c_str());
		ImGui::Text("Vulkan version: ");
		ImGui::SameLine();
		ImGui::Text("%d.%d.%d", gpuInfo.apiVersionMajor, gpuInfo.apiVersionMinor, gpuInfo.apiVersionPatch);
		ImGui::EndChild();

		ImGui::Text("Rendering Information");
		ImGui::BeginChild("##Rendering Information", ImVec2(0, (ImGui::GetWindowSize().y / 2) - ImGui::GetItemRectSize().y * 2.6f));
		ImGui::Text("Trinagles Drawn: ");
		ImGui::SameLine();
		ImGui::Text("%d", Profiler::PofileDataGet<size_t>("TriangleCount"));
		ImGui::Text("Meshlets Drawn: ");
		ImGui::SameLine();
		ImGui::Text("%d", Profiler::PofileDataGet<size_t>("MeshletCount"));
		ImGui::EndChild();
		ImGui::End();
	}
}