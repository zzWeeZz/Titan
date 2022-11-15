#include "TNpch.h"
#include "GraphicsContext.h"

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include "Titan/Application.h"

#include "Titan/Platform/WindowsWindow.h"
#include "d3d12sdklayers.h"
#include "Optick/src/optick.h"

namespace Titan
{

	static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
	{
		switch (messageSeverity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			TN_CORE_TRACE("Validation layer: {0}", pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			TN_CORE_INFO("Validation layer: {0}", pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			TN_CORE_WARN("Validation layer: {0}", pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			TN_CORE_ERROR("Validation layer: {0}", pCallbackData->pMessage);
			break;
		}

		return VK_FALSE;
	}

	void GraphicsContext::Initialize(const GraphicsContextInfo& info)
	{
		s_Info = info;
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Titan";
		appInfo.applicationVersion = VK_API_VERSION_1_3;
		appInfo.pEngineName = "Titan Engine";
		appInfo.engineVersion = VK_API_VERSION_1_3;
		appInfo.apiVersion = VK_API_VERSION_1_3;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;

		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		createInfo.enabledLayerCount = 0;

		std::vector<const char*> extensions;
		for (size_t i = 0; i < glfwExtensionCount; ++i)
		{
			extensions.push_back(glfwExtensions[i]);
		}
		
#ifdef TN_PLATFORM_MACOS
		extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

		createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif
#ifdef TN_CONFIG_DEBUG
		if (info.debuging)
		{
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}
#endif
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();


		const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
#ifdef TN_CONFIG_DEBUG

		if (info.debuging && !CheckValidationLayerSupport(validationLayers))
		{
			TN_CORE_ASSERT(false, "Validation layers requested, but not available!");
		}

		if (info.debuging)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();

			VkDebugUtilsMessengerCreateInfoEXT DebugCreateInfo{};
			PopulateDebugMessengerCreateInfo(DebugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&DebugCreateInfo;
		}
		else
		{
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}
#else
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
#endif

		TN_VK_CHECK(vkCreateInstance(&createInfo, nullptr, &s_Instance));

		uint32_t extensionPropertiesCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionPropertiesCount, nullptr);

		std::vector<VkExtensionProperties> extensionPropoties(extensionPropertiesCount);

		vkEnumerateInstanceExtensionProperties(nullptr, &extensionPropertiesCount, extensionPropoties.data());

		TN_CORE_INFO("Available extensions: ");

		for (auto& extension : extensionPropoties)
		{
			TN_CORE_INFO(extension.extensionName);
		}
#ifdef TN_CONFIG_DEBUG
		if (info.debuging)
		{
			SetupDebugMessenger();
		}
#endif
		CreateSurface();

		s_PhysicalDevice.Create(s_Instance);
		s_Device.Create(s_PhysicalDevice, validationLayers);
		s_Swapchain.Create(s_PhysicalDevice, s_Device);
		auto value = s_PhysicalDevice.FindQueueFamilies().graphicsFamily.value();
		//OPTICK_GPU_INIT_VULKAN(&s_Device.GetHandle(), &s_PhysicalDevice.GetHandle(), &s_Device.GetGraphicsQueue(), &value, 1, nullptr);
	}

	void GraphicsContext::Shutdown()
	{
		s_Device.WaitForIdle();
		s_Swapchain.Shutdown(s_Device);
		s_Device.Shutdown();
#ifdef TN_CONFIG_DEBUG
		if (s_Info.debuging)
		{
			DestroyDebugMessenger();
		}
#endif
		vkDestroySurfaceKHR(s_Instance, s_Surface, nullptr);
		vkDestroyInstance(s_Instance, nullptr);
	}

	bool GraphicsContext::CheckValidationLayerSupport(std::vector<const char*> validationLayer)
	{
		uint32_t layerCount = 0;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : validationLayer)
		{
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound)
			{
				return false;
			}
		}

		return true; 
	}

	void GraphicsContext::CreateSurface()
	{ 
		auto& glfwWindow = *static_cast<GLFWwindow*>(Application::GetWindow().GetNativeWindow());
		glfwCreateWindowSurface(s_Instance, &glfwWindow, nullptr, &s_Surface);
	} 

	void GraphicsContext::SetupDebugMessenger()
	{
		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		PopulateDebugMessengerCreateInfo(createInfo);
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(s_Instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			TN_VK_CHECK(func(s_Instance, &createInfo, nullptr, &s_DebugMessanger));
		}
	}
	void GraphicsContext::DestroyDebugMessenger()
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(s_Instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			func(s_Instance, s_DebugMessanger, nullptr);
		}
	}

	void GraphicsContext::QuerySwapchainSupport(SwapChainSupportDetails& swapchainSupportDetails)
	{
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(s_PhysicalDevice.GetHandle(), s_Surface, &swapchainSupportDetails.capabilities);
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(s_PhysicalDevice.GetHandle(), s_Surface, &formatCount, nullptr);

		if (formatCount != 0)
		{
			swapchainSupportDetails.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(s_PhysicalDevice.GetHandle(), s_Surface, &formatCount, swapchainSupportDetails.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(s_PhysicalDevice.GetHandle(), s_Surface, &presentModeCount, nullptr);
		if (presentModeCount != 0)
		{
			swapchainSupportDetails.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(s_PhysicalDevice.GetHandle(), s_Surface, &presentModeCount, swapchainSupportDetails.presentModes.data());
		}
	}

	bool GraphicsContext::SwapchainAdequate()
	{
		SwapChainSupportDetails details;
		QuerySwapchainSupport(details);

		return !details.formats.empty() && !details.presentModes.empty();
	}

	void GraphicsContext::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
	{
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = VulkanDebugCallback;
		createInfo.pUserData = nullptr;

	}
}