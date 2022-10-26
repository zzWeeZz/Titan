#pragma once
#include <vulkan/vulkan.h>
namespace Titan
{
	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class PhysicalDevice;
	class Device;

	class Swapchain
	{
	public:
		Swapchain() = default;
		~Swapchain() = default;

		void Create(PhysicalDevice& physicalDevice, Device& device);
		int32_t GetNextImage();
		uint32_t GetCurrentImageIndex();
		VkImage& GetImage(size_t index);
		VkImageView& GetCurrentView(size_t index);
		VkSwapchainKHR& GetHandle() { return m_Swapchain; }
		VkExtent2D GetExtent() { return m_SwapchainExtent; }
		VkFormat& GetFormat() { return m_SwapchainFormat; }
		VkSemaphore& GetImageAvailableSemaphore(size_t currentFrame) { return m_ImageAvailableSemaphores[currentFrame]; }
		VkSemaphore& GetRenderFinishedSemaphore(size_t currentFrame) { return m_RenderFinishedSemaphores[currentFrame]; }
		VkFence& GetInFlight(size_t currentFrame) { return m_InFlightFences[currentFrame]; }

		VkFramebuffer& GetFramebuffer(size_t index) { return m_SwapchainFrameBuffers[index]; }
		VkRenderPass& GetRenderPass() { return m_SwapchainRenderPass; }

		void Resize(size_t width, size_t height);
		void CleanUp();
		void Shutdown(Device& device, bool destroyRenderTarget = true);
	private:
		void Validate(PhysicalDevice& physicalDevice, Device& device, int32_t width = -1, int32_t height = -1);
		void InternalResize(size_t width, size_t height);
		void CreateSyncObject();
		void CreateRenderPass();
		void CreateFrameBuffer();
		VkSurfaceFormatKHR ChooseSwapchainFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapchainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		uint32_t m_CurrentImage;

		size_t m_Width;
		size_t m_Height;

		VkSwapchainKHR m_Swapchain;
		std::vector<VkImage> m_SwapchainImages;
		std::vector<VkImageView> m_SwapchainViews;
		VkFormat m_SwapchainFormat;
		VkExtent2D m_SwapchainExtent;

		bool m_NeedsToResize;

		VkRenderPass m_SwapchainRenderPass;
		std::vector<VkFramebuffer> m_SwapchainFrameBuffers;
		PerFrameInFlight<VkSemaphore> m_ImageAvailableSemaphores;
		PerFrameInFlight<VkSemaphore> m_RenderFinishedSemaphores;
		PerFrameInFlight<VkFence> m_InFlightFences;
	};
}