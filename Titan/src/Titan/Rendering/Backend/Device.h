#pragma once
#include <vulkan/vulkan.h>
#include "Titan/Rendering/Backend/PhysicalDevice.h"
namespace Titan
{
	class Device
	{
	public:
		void Create(PhysicalDevice& pysicalDevice, const std::vector<const char*> validationLayer);
		VkDevice& GetHandle() { return m_Device; }
		void Shutdown();
	private:
		bool CheckDeviceExtensionSupport(PhysicalDevice& physicalDevice, const std::vector<const char*> deviceExtensions);
		VkDevice m_Device;
		VkQueue m_GraphicsQueue;
		VkQueue m_PresentQueue;
	};
}