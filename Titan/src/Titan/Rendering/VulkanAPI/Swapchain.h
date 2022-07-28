#pragma once
#include "Titan/Core/Core.h"
#include "VulkanUtils/VulkanStructs.h"
#include <vulkan/vulkan.h>

#include "CommandBuffer.h"
#include "RenderPass.h"
#include "Buffers/Buffers.h"
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
		inline VkImageView& GetDepthView() { return m_DepthImageView; }
		inline uint32_t& GetImageCount() { return m_ImageCount; }
		inline Ref<RenderPass> GetRenderPass() { return m_SwapchainRenderPass; }
		void Submit(std::vector<Ref<CommandBuffer>>& commandBuffers);
		void WaitOnFences(bool waitAndReset = true);
		void Present();
		void ShutDown();
		static Ref<Swapchain> Create();
	private:
		void InitializeSyncStructure();

		uint32_t m_ImageCount = 0u;

		Ref<RenderPass> m_SwapchainRenderPass;

		VkSwapchainKHR m_SwapChain;
		VkFormat m_SwapchainImageFormat;
		std::vector<VkImage> m_SwapchainImages;
		std::vector<VkImageView> m_SwapchainImageViews;

		VkImageView m_DepthImageView;
		AllocatedImage m_DepthImage;
		VkFormat m_DepthFormat;


		VkSemaphore m_PresentSemaphore;
		VkSemaphore m_RenderSemaphore;
		VkFence m_RenderFence;
	};
}
