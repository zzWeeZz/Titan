#include "GraphicsContext.h"
#include "VkBootstrap.h"
#include "GLFW/glfw3.h"
#include "Titan/Application.h"
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

	void GraphicsContext::Initialize()
	{
		if (glfwVulkanSupported() == GLFW_FALSE)
		{
			TN_CORE_ASSERT(false, "Vulkan is not supported");
		}
		vkb::InstanceBuilder builder;
		auto instanceRect = builder.set_app_name("Titan")
			.request_validation_layers(true)
			.set_debug_callback(VulkanDebugCallback)
			.require_api_version(1, 1).build();

		vkb::Instance vkbinstance = instanceRect.value();
		m_Instance = vkbinstance.instance;

		auto& glfwWindow = *static_cast<GLFWwindow*>(Application::GetWindow().GetNativeWindow());
		glfwCreateWindowSurface(m_Instance, &glfwWindow, nullptr, &m_Surface);

		vkb::PhysicalDeviceSelector selector{ vkbinstance };
		vkb::PhysicalDevice physicalDevice = selector.set_minimum_version(1, 1).set_surface(m_Surface).select().value();
		vkb::DeviceBuilder deviceBuilder{ physicalDevice };
		vkb::Device device = deviceBuilder.build().value();

		m_Device = device.device;
		m_PhysicalDevice = physicalDevice.physical_device;
		m_DebugMessenger = instanceRect->debug_messenger;

		m_GraphicsQueue = device.get_queue(vkb::QueueType::graphics).value();
		m_GraphicsQueueFamily = device.get_queue_index(vkb::QueueType::graphics).value();

		VmaAllocatorCreateInfo allocatorInfo = {};
		allocatorInfo.physicalDevice = m_PhysicalDevice;
		allocatorInfo.device = m_Device;
		allocatorInfo.instance = m_Instance;
		vmaCreateAllocator(&allocatorInfo, &m_Allocator);

		m_Swapchain = Swapchain::Create();

		VkFenceCreateInfo uploadFenceCreateInfo{};
		uploadFenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		uploadFenceCreateInfo.pNext = nullptr;

		TN_VK_CHECK(vkCreateFence(m_Device, &uploadFenceCreateInfo, nullptr, &m_UploadContext.uploadFence));

		GlobalDeletionQueue.PushFunction([=]
			{
				vkDestroyFence(m_Device, m_UploadContext.uploadFence, nullptr);
			});
		Ref<CommandBuffer> cmdBuff = CommandBuffer::Create(m_GraphicsQueue, m_GraphicsQueueFamily);
		m_UploadContext.commandBuffer = cmdBuff->GetHandle();
		m_UploadContext.commandPool = cmdBuff->GetPool();
	}

	void GraphicsContext::CreateCommandBuffer(Ref<CommandBuffer>& outCommandBuffer)
	{
		outCommandBuffer = CommandBuffer::Create(m_GraphicsQueue, m_GraphicsQueueFamily);
		GlobalDeletionQueue.PushFunction([=]
			{
				outCommandBuffer->Shutdown();
			});
	}


	void GraphicsContext::ImmediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function)
	{
		VkCommandBuffer cmd = m_UploadContext.commandBuffer;

		VkCommandBufferBeginInfo cmdBeginInfo{};
		cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBeginInfo.pNext = nullptr;
		cmdBeginInfo.pInheritanceInfo = nullptr;
		cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		TN_VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));
		function(cmd);
		TN_VK_CHECK(vkEndCommandBuffer(cmd));

		VkSubmitInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		info.pNext = nullptr;

		info.waitSemaphoreCount = 0;
		info.pWaitSemaphores = nullptr;
		info.pWaitDstStageMask = nullptr;
		info.commandBufferCount = 1;
		info.pCommandBuffers = &cmd;
		info.signalSemaphoreCount = 0;
		info.pSignalSemaphores = nullptr;

		TN_VK_CHECK(vkQueueSubmit(m_GraphicsQueue, 1, &info, m_UploadContext.uploadFence));
		vkWaitForFences(m_Device, 1, &m_UploadContext.uploadFence, true, UINTMAX_MAX);
		vkResetFences(m_Device, 1, &m_UploadContext.uploadFence);

		vkResetCommandPool(m_Device, m_UploadContext.commandPool, 0);
	}

	void GraphicsContext::ShutDown()
	{
		m_Swapchain->WaitOnFences(false);

		GlobalDeletionQueue.Flush();
		vkb::destroy_debug_utils_messenger(m_Instance, m_DebugMessenger);
		vmaDestroyAllocator(m_Allocator);
		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);

		vkDestroyDevice(m_Device, nullptr);
		vkDestroyInstance(m_Instance, nullptr);
	}
}
