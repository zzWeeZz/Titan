#include "GraphicsContext.h"
#include "VkBootstrap.h"
#include "GLFW/glfw3.h"
#include "Titan/Application.h"
namespace Titan
{
	void GraphicsContext::Initialize()
	{
		if (glfwVulkanSupported() == GLFW_FALSE)
		{
			TN_CORE_ASSERT(false, "Vulkan is not supported");
		}
		vkb::InstanceBuilder builder;
		auto instanceRect = builder.set_app_name("Titan")
			.request_validation_layers(true)
			.require_api_version(1, 1).build();

		vkb::Instance vkbinstance = instanceRect.value();
		m_Instance = vkbinstance.instance;

		auto& glfwWindow = *static_cast<GLFWwindow*>(Application::GetWindow().GetNativeWindow());
		glfwCreateWindowSurface(m_Instance, &glfwWindow, nullptr, &m_Surface);

		vkb::PhysicalDeviceSelector selector{ vkbinstance };
		vkb::PhysicalDevice physicalDevice = selector.set_minimum_version(1,1).set_surface(m_Surface).select().value();
		vkb::DeviceBuilder deviceBuilder{ physicalDevice };
		vkb::Device device = deviceBuilder.build().value();

		m_Device = device.device;
		m_PhysicalDevice = physicalDevice.physical_device;

		m_GraphicsQueue = device.get_queue(vkb::QueueType::graphics).value();
		m_GraphicsQueueFamily = device.get_queue_index(vkb::QueueType::graphics).value();

		m_Swapchain = Swapchain::Create();
	}
	

	void GraphicsContext::ShutDown()
	{
		m_MainDeletionQueue.Flush();
		m_Swapchain->ShutDown();
		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
		vkDestroyDevice(m_Device, nullptr);
		vkDestroyInstance(m_Instance, nullptr);
	}
}
