#include "TNpch.h"
#include "TitanImGui.h"

#include "ImGui/imgui.h"
#include "Titan/Rendering/GraphicsContext.h"
#include "Titan/Application.h"
#include "Titan/Core/Core.h"
#include "vulkan/vulkan.h"
#include "ImGui/backends/imgui_impl_glfw.h"
#include "ImGui/backends/imgui_impl_vulkan.h"
#include "Titan/Utils/TitanAllocator.h"
namespace Titan
{
	static std::vector<VkCommandBuffer> s_ImGuiCommandBuffer;

	void TitanImGui::Initialize()
	{
		VkDescriptorPoolSize poolSizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};
		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000;
		pool_info.poolSizeCount = std::size(poolSizes);
		pool_info.pPoolSizes = poolSizes;

		VkDescriptorPool imguiPool;
		TN_VK_CHECK(vkCreateDescriptorPool(GraphicsContext::GetDevice().GetHandle(), &pool_info, nullptr, &imguiPool));

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui_ImplGlfw_InitForVulkan(static_cast<GLFWwindow*>(Application::GetWindow().GetNativeWindow()), true);
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = GraphicsContext::GetInstance();
		init_info.PhysicalDevice = GraphicsContext::GetPhysicalDevice().GetHandle();
		init_info.Device = GraphicsContext::GetDevice().GetHandle();
		init_info.Queue = GraphicsContext::GetDevice().GetGraphicsQueue();

		init_info.DescriptorPool = imguiPool;
		init_info.MinImageCount = 3;
		init_info.ImageCount = 3;
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

		ImGui_ImplVulkan_Init(&init_info, GraphicsContext::GetSwapchain().GetRenderPass());
		ImGui_ImplVulkan_DestroyFontUploadObjects();
		TitanAllocator::QueueDeletion([=]
			{
				vkDeviceWaitIdle(GraphicsContext::GetDevice().GetHandle());
				vkDestroyDescriptorPool(GraphicsContext::GetDevice().GetHandle(), imguiPool, nullptr);
				ImGui_ImplVulkan_Shutdown();
			});

		ImGuiIO& io = ImGui::GetIO();

		io.Fonts->Clear();
		io.Fonts->AddFontFromFileTTF("Assets/Fonts/OpenSans-Light.ttf", 16);
		io.Fonts->AddFontFromFileTTF("Assets/Fonts/OpenSans-Regular.ttf", 16);
		io.Fonts->AddFontFromFileTTF("Assets/Fonts/OpenSans-Light.ttf", 32);
		io.Fonts->AddFontFromFileTTF("Assets/Fonts/OpenSans-Regular.ttf", 11);
		io.Fonts->AddFontFromFileTTF("Assets/Fonts/OpenSans-Bold.ttf", 11);
		io.Fonts->Build();
		ImGuiStyle* style = &ImGui::GetStyle();

		style->Alpha = 1;
		style->WindowPadding = ImVec2(15, 15);
		style->WindowRounding = 5.0f;
		style->FramePadding = ImVec2(5, 5);
		style->FrameRounding = 4.0f;
		style->ItemSpacing = ImVec2(12, 8);
		style->ItemInnerSpacing = ImVec2(8, 6);
		style->IndentSpacing = 25.0f;
		style->ScrollbarSize = 15.0f;
		style->ScrollbarRounding = 9.0f;
		style->GrabMinSize = 5.0f;
		style->GrabRounding = 3.0f;

		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
		colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.21f, 0.21f, 0.21f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.16f, 0.16f, 0.16f, 0.40f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.26f, 0.26f, 0.67f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.54f, 0.03f, 0.03f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.19f, 0.19f, 0.19f, 0.40f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.23f, 0.23f, 0.23f, 0.31f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.10f, 0.10f, 0.10f, 0.80f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.59f, 0.59f, 0.59f, 0.78f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.66f, 0.00f, 0.00f, 0.20f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.16f, 0.00f, 0.00f, 0.67f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.30f, 0.00f, 0.00f, 0.95f);
		colors[ImGuiCol_Tab] = ImVec4(0.19f, 0.19f, 0.19f, 0.86f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.13f, 0.13f, 0.13f, 0.80f);
		colors[ImGuiCol_TabActive] = ImVec4(0.40f, 0.04f, 0.04f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.11f, 0.11f, 0.11f, 0.97f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.27f, 0.27f, 0.28f, 1.00f);
		colors[ImGuiCol_DockingPreview] = ImVec4(0.34f, 0.00f, 0.00f, 0.70f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.43f, 0.00f, 0.00f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.48f, 0.02f, 0.02f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);


		GraphicsContext::GetDevice().ImmediateSubmit([&](VkCommandBuffer cmd)
			{
				ImGui_ImplVulkan_CreateFontsTexture(cmd);
			});
		s_ImGuiCommandBuffer.resize(g_FramesInFlight);
		for (size_t i = 0; i < g_FramesInFlight; ++i)
		{
			s_ImGuiCommandBuffer[i] = GraphicsContext::GetDevice().CreateSecondaryCommandBuffer();
		}
	}

	void TitanImGui::Begin()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void TitanImGui::End()
	{
		ImGui::Render();

		auto& swapchain = GraphicsContext::GetSwapchain();
		VkCommandBuffer cmd;
		VkCommandBuffer secondaryCmd;
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandBufferCount = 1;
		allocInfo.commandPool = GraphicsContext::GetDevice().GetCommandPool(0);
		vkAllocateCommandBuffers(GraphicsContext::GetDevice().GetHandle(), &allocInfo, &cmd);
		secondaryCmd = s_ImGuiCommandBuffer[GraphicsContext::GetCurrentFrame()];
		{
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; // Optional
			beginInfo.pInheritanceInfo = nullptr; // Optiona

			TN_VK_CHECK(vkBeginCommandBuffer(cmd, &beginInfo));
		}

		{
			VkClearValue clearValues[2];
			clearValues[0].color = { { 0.1f, 0.1f, 0.1f, 1.f } };
			clearValues[1].depthStencil = { 1.0f, 0 };

			VkRenderPassBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			beginInfo.pNext = nullptr;
			beginInfo.renderPass = swapchain.GetRenderPass();
			beginInfo.renderArea.offset.x = 0;
			beginInfo.renderArea.offset.y = 0;
			beginInfo.renderArea.extent.width = Application::GetWindow().GetWidth();
			beginInfo.renderArea.extent.height = Application::GetWindow().GetHeight();
			beginInfo.clearValueCount = 2;
			beginInfo.pClearValues = clearValues;
			beginInfo.framebuffer = swapchain.GetFramebuffer(swapchain.GetCurrentImageIndex());

			vkCmdBeginRenderPass(cmd, &beginInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
		}

		{
			VkCommandBufferInheritanceInfo inheritInfo{};
			inheritInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
			inheritInfo.renderPass = swapchain.GetRenderPass();
			inheritInfo.framebuffer = swapchain.GetFramebuffer(swapchain.GetCurrentImageIndex());

			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
			beginInfo.pInheritanceInfo = &inheritInfo;

			TN_VK_CHECK(vkBeginCommandBuffer(secondaryCmd, &beginInfo));
		}

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(Application::GetWindow().GetWidth());
		viewport.height = static_cast<float>(Application::GetWindow().GetHeight());
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(secondaryCmd, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = swapchain.GetExtent();
		vkCmdSetScissor(secondaryCmd, 0, 1, &scissor);


		ImDrawData* drawData = ImGui::GetDrawData();
		ImGui_ImplVulkan_RenderDrawData(drawData, secondaryCmd);

		vkEndCommandBuffer(secondaryCmd);

		vkCmdExecuteCommands(cmd, 1, &secondaryCmd);

		vkCmdEndRenderPass(cmd);
		TN_VK_CHECK(vkEndCommandBuffer(cmd));
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
		vkFreeCommandBuffers(GraphicsContext::GetDevice().GetHandle(), GraphicsContext::GetDevice().GetCommandPool(0), 1, &cmd);
	}
}
