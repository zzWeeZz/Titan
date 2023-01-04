#include "RenderingStatsPanel.h"

#include <ImGui/imgui.h>

#define TN_PROFILE_NO_OPTICK
#include "Titan/Utils/Chrono.h"
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
		auto& gpuInfo = GraphicsContext::GetPhysicalDevice().GetGPUInfo();

		if (ImGui::CollapsingHeader("General"))
		{

			ImGui::Text("GPU: ");
			ImGui::SameLine();
			ImGui::Text(gpuInfo.gpuName.c_str());
			ImGui::Text("GPU driver version: ");
			ImGui::SameLine();
			ImGui::Text("%d.%d.%d", gpuInfo.gpuDriverVersionMajor, gpuInfo.gpuDriverVersionMinor, gpuInfo.gpuDriverVersionPatch);
			ImGui::Text("Vulkan version: ");
			ImGui::SameLine();
			ImGui::Text("%d.%d.%d", gpuInfo.apiVersionMajor, gpuInfo.apiVersionMinor, gpuInfo.apiVersionPatch);
		}

		if (ImGui::CollapsingHeader("Mesh shader API"))
		{
			ImGui::Text("Max tasks per draw: ");
			ImGui::SameLine();
			ImGui::Text("%d", gpuInfo.gpuMaxDrawTasksCount);
			ImGui::Text("Max task work group invocations: ");
			ImGui::SameLine();
			ImGui::Text("%d", gpuInfo.gpuMaxTaskWorkGroupInvocations);
			ImGui::Text("Max mesh output vertices: ");
			ImGui::SameLine();
			ImGui::Text("%d", gpuInfo.gpuMAxMeshOutputVertices);
			ImGui::Text("Max mesh output primatives: ");
			ImGui::SameLine();
			ImGui::Text("%d", gpuInfo.gpuMAxMeshOutputPrimatives);
		}

		if (ImGui::CollapsingHeader("Allocator"))
		{
			ImGui::Text("VRAM Allocated: ");
			ImGui::SameLine();
			auto allocated = Profiler::PofileDataGet<uint64_t>("BytesAllocated");
			allocated /= 1000000;
			ImGui::Text("%d mb", allocated);
		}
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
		ImGui::Text("FPS: ");
		ImGui::SameLine();
		ImGui::Text("%f", 1.f / Chrono::Timestep());
		ImGui::Text("ms: ");
		ImGui::SameLine();
		ImGui::Text("%f", Chrono::Timestep() * 1000.f);
		ImGui::EndChild();
		ImGui::End();
	}
}