#pragma once
#include "Titan/Core/Core.h"
#include "VulkanUtils/VulkanStructs.h"
#include <vulkan/vulkan.h>

#include "Titan/Window.h"

namespace Titan
{
	class Swapchain
	{
	public:
		Swapchain();
		inline VkSwapchainKHR& Get() { return m_SwapChain; }

		void ShutDown();
		static Ref<Swapchain> Create();
	private:
		VkSwapchainKHR m_SwapChain;
		VkFormat m_SwapchainImageFormat;
		std::vector<VkImage> m_SwapchainImages;
		std::vector<VkImageView> m_SwapchainImageViews;

		DeletionQueue m_DeletionQueue;
	};
}
