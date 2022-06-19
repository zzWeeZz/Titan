#pragma once
#include "Titan/Core/Core.h"
#include "VulkanUtils/VulkanStructs.h"
#include <vulkan/vulkan.h>

#include "CommandBuffer.h"
#include "Titan/Window.h"

namespace Titan
{
	class Swapchain
	{
	public:
		Swapchain();
		inline VkSwapchainKHR& Get() { return m_SwapChain; }
		inline VkFormat& GetFormat() { return m_SwapchainImageFormat; }
		inline std::vector<VkImage>& GetImages() { return m_SwapchainImages; }
		inline std::vector<VkImageView>& GetViews() { return m_SwapchainImageViews; }
		inline uint32_t& GetImageCount() { return m_ImageCount; }
		void Submit(std::vector<Ref<CommandBuffer>>& commandBuffers);
		void WaitOnFences(bool waitAndReset = true);
		void Present();
		void ShutDown();
		static Ref<Swapchain> Create();
	private:
		void InitializeSyncStructure();

		uint32_t m_ImageCount = 0u;

		VkSwapchainKHR m_SwapChain;
		VkFormat m_SwapchainImageFormat;
		std::vector<VkImage> m_SwapchainImages;
		std::vector<VkImageView> m_SwapchainImageViews;

		VkSemaphore m_PresentSemaphore;
		VkSemaphore m_RenderSemaphore;
		VkFence m_RenderFence;

		DeletionQueue m_DeletionQueue;
	};
}
