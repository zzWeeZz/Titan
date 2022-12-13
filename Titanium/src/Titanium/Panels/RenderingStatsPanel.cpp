#include "RenderingStatsPanel.h"

#include <ImGui/imgui.h>

#define TN_PROFILE_NO_OPTICK
#include "Titan/Utils/Profiler.h"

#include "Titan/Rendering/Renderer.h"
#include "Titan/Rendering/GraphicsContext.h"

namespace Titan
{
	void RenderingStatsPanel::OnImGuiDraw()
	{
		ImGui::Begin("Rendering stats");

		ImGui::Text("GPU Information");
		ImGui::BeginChild("##GPU Information", ImVec2(0, ((ImGui::GetWindowSize().y / 2) - 2 * (ImGui::GetItemRectSize().y + ImGui::GetStyle().ItemSpacing.y)) - 0.500001937155f));
		ImGui::Text("GPU: ");
		ImGui::SameLine();
		auto& gpuInfo = GraphicsContext::GetPhysicalDevice().GetGPUInfo();
		ImGui::Text(gpuInfo.gpuName.c_str());
		ImGui::Text("GPU driver version: ");
		ImGui::SameLine();
		ImGui::Text("%d.%d.%d", gpuInfo.gpuDriverVersionMajor, gpuInfo.gpuDriverVersionMinor, gpuInfo.gpuDriverVersionPatch);
		ImGui::Text("Vulkan version: ");
		ImGui::SameLine();
		ImGui::Text("%d.%d.%d", gpuInfo.apiVersionMajor, gpuInfo.apiVersionMinor, gpuInfo.apiVersionPatch);
		ImGui::Text("VRAM Allocated: ");
		ImGui::SameLine();
		ImGui::Text("%d", Profiler::PofileDataGet<uint64_t>("BytesAllocated"));
		ImGui::EndChild();

		ImGui::Text("Rendering Information");
		ImGui::SameLine();
		static bool checkBox = false;
		ImGui::Checkbox("Debug meshlets", &checkBox);
		Renderer::SetDebugLayer(static_cast<uint32_t>(checkBox));
		ImGui::BeginChild("##Rendering Information");
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