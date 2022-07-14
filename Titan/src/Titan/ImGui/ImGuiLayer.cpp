#include "ImGuiLayer.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "Titan/Application.h"
#include "Titan/Core/Core.h"
#include "Titan/Rendering/VulkanAPI/GraphicsContext.h"
#include "Titan/Rendering/VulkanAPI/VulkanRenderer.h"
#include "vulkan/vulkan.h"
namespace Titan
{
	void ImGuiLayer::OnAttach()
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
		TN_VK_CHECK(vkCreateDescriptorPool(GraphicsContext::GetDevice(), &pool_info, nullptr, &imguiPool));

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui_ImplGlfw_InitForVulkan(static_cast<GLFWwindow*>(Application::GetWindow().GetNativeWindow()), true);
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = GraphicsContext::GetInstance();
		init_info.PhysicalDevice = GraphicsContext::GetPhysicalDevice();
		init_info.Device = GraphicsContext::GetDevice();
		init_info.Queue = GraphicsContext::GetGraphicsQueue();
		
		init_info.DescriptorPool = imguiPool;
		init_info.MinImageCount = 3;
		init_info.ImageCount = 3;
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

		ImGui_ImplVulkan_Init(&init_info, VulkanRenderer::GetRenderPass()->GetRenderPass());
		ImGui_ImplVulkan_DestroyFontUploadObjects();
		GlobalDeletionQueue.PushFunction([=]
			{
				vkDestroyDescriptorPool(GraphicsContext::GetDevice(), imguiPool, nullptr);
				ImGui_ImplVulkan_Shutdown();
			});
		
		ImGuiIO& io = ImGui::GetIO();

		io.Fonts->Clear();
		io.Fonts->AddFontFromFileTTF("Fonts/OpenSans-Light.ttf", 16);
		io.Fonts->AddFontFromFileTTF("Fonts/OpenSans-Regular.ttf", 16);
		io.Fonts->AddFontFromFileTTF("Fonts/OpenSans-Light.ttf", 32);
		io.Fonts->AddFontFromFileTTF("Fonts/OpenSans-Regular.ttf", 11);
		io.Fonts->AddFontFromFileTTF("Fonts/OpenSans-Bold.ttf", 11);
		io.Fonts->Build();
	
		GraphicsContext::ImmediateSubmit([&](VkCommandBuffer cmd)
			{
				ImGui_ImplVulkan_CreateFontsTexture(cmd);
			});

	}

	void ImGuiLayer::OnUpdate()
	{
		Layer::OnUpdate();
	}

	void ImGuiLayer::OnDetach()
	{
		Layer::OnDetach();
	}

	void ImGuiLayer::OnEvent(Event& e)
	{
		Layer::OnEvent(e);
	}
}
