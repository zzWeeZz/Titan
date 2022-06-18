#pragma once
#include <vulkan/vulkan.h>

#include "Swapchain.h"
#include "VulkanUtils/VulkanStructs.h"
#include "Titan/Core/Core.h"
#include "Titan/Window.h"
namespace Titan
{
	class GraphicsContext
	{
	public:
		static void Initialize();
		inline static VkDevice& GetDevice() { return m_Device; }
		inline static VkPhysicalDevice& GetPhysicalDevice() { return m_PhysicalDevice; }
		inline static VkSurfaceKHR& GetSurface() { return m_Surface; }
		inline static Swapchain& GetSwapchain() { return *m_Swapchain.get(); }
		static void ShutDown();
	private:
		inline static DeletionQueue m_MainDeletionQueue;

		inline static VkDevice m_Device;
		inline static VkPhysicalDevice m_PhysicalDevice;

		inline static VkInstance m_Instance;
		inline static VkSurfaceKHR m_Surface;

		inline static VkQueue m_GraphicsQueue;
		inline static uint32_t m_GraphicsQueueFamily;

		inline static Ref<Swapchain> m_Swapchain;
	};
}
