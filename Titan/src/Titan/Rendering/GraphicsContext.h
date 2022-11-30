#pragma once
#include <array>

#include <vulkan/vulkan.h>

#include "Titan/Core/Core.h"

#include <Titan/Rendering/Backend/Device.h>
#include <Titan/Rendering/Backend/Swapchain.h>
#include <Titan/Rendering/Backend/PhysicalDevice.h>
#include <Titan/Rendering/Pipeline/GraphicsPipeline.h>

namespace Titan
{
	constexpr uint32_t g_FrameCount = 3;

	struct GraphicsContextInfo
	{
		uint32_t width = 1280;
		uint32_t height = 720;
		bool debuging = true;
	};

	class GraphicsContext
	{
		friend class Swapchain;
	public:
		static uint32_t& GetCurrentFrame() { return s_CurrentFrame; }
		static void Initialize(const GraphicsContextInfo& info);
		static const GraphicsContextInfo& GetInfo() { return s_Info; }
		static Device& GetDevice() { return s_Device; }
		static PhysicalDevice& GetPhysicalDevice() { return s_PhysicalDevice; }
		static Swapchain& GetSwapchain() { return s_Swapchain; }
		static VkSurfaceKHR& GetSurface() { return s_Surface; }
		static VkInstance& GetInstance() { return s_Instance; }
		static bool SwapchainAdequate();
		static void Shutdown();
	private:
		static bool CheckValidationLayerSupport(std::vector<const char*> validationLayer);
		static void CreateSurface();
		static void SetupDebugMessenger();
		static void DestroyDebugMessenger();
		static void QuerySwapchainSupport(SwapChainSupportDetails& swapchainSupportDetails);
		static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

		inline static GraphicsContextInfo s_Info;
		inline static PhysicalDevice s_PhysicalDevice;
		inline static Device s_Device;
		inline static Swapchain s_Swapchain;

		inline static VkInstance s_Instance;
		inline static VkSurfaceKHR s_Surface;
		inline static VkDebugUtilsMessengerEXT s_DebugMessanger;
		inline static uint32_t s_CurrentFrame;
	};
}