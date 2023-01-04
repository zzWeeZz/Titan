#include "TNpch.h"
#include "Device.h"
#include "Titan/Rendering/GraphicsContext.h"
#include "Titan/Utils/TitanAllocator.h"
namespace Titan
{
	void Device::Create(PhysicalDevice& physicalDevice, const std::vector<const char*> validationLayers)
	{
		QueueFamilyIndices indices = physicalDevice.FindQueueFamilies();
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo{};

			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceVulkan12Features device12Features{};
		device12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
		device12Features.shaderInt8 = VK_TRUE;

		VkPhysicalDeviceFeatures2 deviceFeatures{};
		deviceFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		deviceFeatures.pNext = &device12Features;

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.pEnabledFeatures =nullptr;
		createInfo.pNext = &deviceFeatures;

		createInfo.enabledExtensionCount = 0;
#ifdef TN_CONFIG_DEBUG
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
#else
		createInfo.enabledLayerCount = 0;
#endif

		const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_NV_MESH_SHADER_EXTENSION_NAME, "VK_KHR_maintenance4"};
		bool extensionsSupported = CheckDeviceExtensionSupport(physicalDevice, deviceExtensions);
		if (!(indices.HasValue() && extensionsSupported && GraphicsContext::SwapchainAdequate()))
		{
			TN_CORE_ASSERT(false, "Device does not support a vulkan swapchain!");
		}
		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();
		
		const VkPhysicalDeviceMaintenance4Features maintenance4Feature
		{
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_FEATURES,
		.maintenance4 = VK_TRUE
		};

		const VkPhysicalDeviceMeshShaderFeaturesNV meshShaderPropFeature
		{
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_NV,
		.pNext = (void*)&maintenance4Feature,
		.taskShader = VK_TRUE,
		.meshShader = VK_TRUE,
		
		};
		
		const VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeature
		{
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES,
		.pNext = (void*)&meshShaderPropFeature,
		.dynamicRendering = VK_TRUE,
		};

		createInfo.pNext = &dynamicRenderingFeature;


		TN_VK_CHECK(vkCreateDevice(physicalDevice.GetHandle(), &createInfo, nullptr, &m_Device));

		vkGetDeviceQueue(m_Device, indices.graphicsFamily.value(), 0, &m_GraphicsQueue);
		vkGetDeviceQueue(m_Device, indices.presentFamily.value(), 0, &m_PresentQueue);
		CreateCommandPools(physicalDevice);
	}

	void Device::WaitForIdle()
	{
		vkDeviceWaitIdle(m_Device);
	}

	VkCommandBuffer Device::CreateSecondaryCommandBuffer()
	{
		VkCommandBuffer commandBuffer;
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = m_CommandPools[0];
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
		allocInfo.commandBufferCount = 1;

		TN_VK_CHECK(vkAllocateCommandBuffers(m_Device, &allocInfo, &commandBuffer));
		return commandBuffer;
	}

	void Device::ImmediateSubmit(std::function<void(VkCommandBuffer cmd)>&& func)
	{
		VkCommandBuffer cmd;
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandBufferCount = 1;
		allocInfo.commandPool = m_CommandPools[0];
		vkAllocateCommandBuffers(m_Device, &allocInfo, &cmd);

		auto& swapchain = GraphicsContext::GetSwapchain();
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optiona

		VkFence fence;
		VkFenceCreateInfo fenceCreateInfo{};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = 0;
		vkCreateFence(m_Device, &fenceCreateInfo, nullptr, &fence);

		TN_VK_CHECK(vkBeginCommandBuffer(cmd, &beginInfo));
		func(cmd);
		TN_VK_CHECK(vkEndCommandBuffer(cmd));

		VkSubmitInfo info{};
		info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		info.pNext = nullptr;
		info.waitSemaphoreCount = 0;
		info.pWaitSemaphores = nullptr;
		info.pWaitDstStageMask = nullptr;
		info.commandBufferCount = 1;
		info.pCommandBuffers = &cmd;
		info.signalSemaphoreCount = 0;
		info.pSignalSemaphores = nullptr;

		TN_VK_CHECK(vkQueueSubmit(m_GraphicsQueue, 1, &info, fence));

		vkWaitForFences(m_Device, 1, &fence, true, UINT64_MAX);
		vkResetFences(m_Device, 1, &fence);

		vkResetCommandBuffer(cmd, 0);
		vkDestroyFence(m_Device, fence, nullptr);
		vkFreeCommandBuffers(m_Device, m_CommandPools[0], 1, &cmd);
	}

	void Device::Shutdown()
	{
		vkDestroyDevice(m_Device, nullptr);
	}

	void Device::CreateCommandPools(PhysicalDevice& physicalDevice, size_t amount)
	{
		QueueFamilyIndices indices = physicalDevice.FindQueueFamilies();
		m_CommandPools.resize(amount);
		m_CommandBuffers.resize(amount);
		
		for (size_t i = 0; i < amount; ++i)
		{
			VkCommandPoolCreateInfo poolInfo{};
			poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			poolInfo.queueFamilyIndex = indices.graphicsFamily.value();

			TN_VK_CHECK(vkCreateCommandPool(m_Device, &poolInfo, nullptr, &m_CommandPools[i]));

			VkCommandBufferAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.commandPool = m_CommandPools[i];
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandBufferCount = g_FramesInFlight;

			TN_VK_CHECK(vkAllocateCommandBuffers(m_Device, &allocInfo, m_CommandBuffers[i].data()));
		}
		for (size_t i = 0; i < amount; ++i)
		{
			TitanAllocator::QueueDeletion([&, i]() { vkDestroyCommandPool(m_Device, m_CommandPools[i], nullptr); });
		}
	}

	bool Device::CheckDeviceExtensionSupport(PhysicalDevice& physicalDevice, const std::vector<const char*> deviceExtensions)
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(physicalDevice.GetHandle(), nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(physicalDevice.GetHandle(), nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
		for (const auto& extension : availableExtensions)
		{
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}
}