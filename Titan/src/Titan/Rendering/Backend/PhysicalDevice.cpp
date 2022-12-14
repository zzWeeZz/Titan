#include "TNpch.h"
#include "PhysicalDevice.h"
#include "Titan/Rendering/GraphicsContext.h"
namespace Titan
{
    void PhysicalDevice::Create(VkInstance& instance)
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        if (deviceCount == 0)
        {
            TN_CORE_ASSERT(false, "Failed to find GPU with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        // find a device that is suitable
        std::vector<std::pair<uint32_t, VkPhysicalDevice>> candidates;
        for (const auto& device : devices)
        {
            VkPhysicalDeviceProperties deviceProperties;
            vkGetPhysicalDeviceProperties(device, &deviceProperties);

			VkPhysicalDeviceFeatures deviceFeatures;
			vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
            uint32_t score = 0;
            if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                score += 1000;
            }

            score += deviceProperties.limits.maxImageDimension2D;

            if (!deviceFeatures.geometryShader)
            {
                score = 0;
            }

            candidates.emplace_back(score, device);
        }

        if (candidates.rbegin()->first > 0)
        {
            m_PhysicalDevice = candidates.rbegin()->second;
			VkPhysicalDeviceProperties deviceProperties;
			vkGetPhysicalDeviceProperties(m_PhysicalDevice, &deviceProperties);
            m_GPUInfo.gpuName = deviceProperties.deviceName;
            m_GPUInfo.apiVersionMajor = VK_VERSION_MAJOR(deviceProperties.apiVersion);
            m_GPUInfo.apiVersionMinor = VK_VERSION_MINOR(deviceProperties.apiVersion);
            m_GPUInfo.apiVersionPatch = VK_VERSION_PATCH(deviceProperties.apiVersion);

			m_GPUInfo.gpuDriverVersionMajor = VK_VERSION_MAJOR(deviceProperties.driverVersion);
			m_GPUInfo.gpuDriverVersionMinor = VK_VERSION_MINOR(deviceProperties.driverVersion);
			m_GPUInfo.gpuDriverVersionPatch = VK_VERSION_PATCH(deviceProperties.driverVersion);
        }
    }
    QueueFamilyIndices PhysicalDevice::FindQueueFamilies()
    {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);

        vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, queueFamilies.data());

        int32_t i = 0;
        for (const auto& queueFamily : queueFamilies)
        {
            
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.graphicsFamily = i;
                VkBool32 presentSupport = false;
                vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, i, GraphicsContext::GetSurface(), &presentSupport);
                if (presentSupport)
                {
                    indices.presentFamily = i;
                }
            }
			if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT)
			{
				indices.transferFamily = i;
			}
			if (indices.HasValue())
				break;
            i++;
        }


        return indices;
    }
}