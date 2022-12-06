#include "RenderingStatsPanel.h"

#include <ImGui/imgui.h>
#include "Titan/Rendering/GraphicsContext.h"

namespace Titan
{
	void RenderingStatsPanel::OnImGuiDraw()
	{
		ImGui::Begin("Rendering stats");
		ImGui::Text("GPU Information");
		ImGui::BeginChild("##GPU Information");
		ImGui::Text("GPU: ");
		ImGui::SameLine();
		auto gpuInfo = GraphicsContext::GetPhysicalDevice().GetGPUInfo();
		ImGui::Text(gpuInfo.gpuName.c_str());
		ImGui::Text("Vulkan version: ");
		ImGui::SameLine();
		ImGui::Text("%d.%d.%d", gpuInfo.apiVersionMajor, gpuInfo.apiVersionMinor, gpuInfo.apiVersionPatch);
		ImGui::EndChild();
		ImGui::End();
	}
}