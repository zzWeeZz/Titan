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
		VkImage& GetImage(size_t index);
		VkImageView& GetCurrentView(size_t index);
		VkSwapchainKHR& GetHandle() { return m_Swapchain; }
		VkExtent2D GetExtent() { return m_SwapchainExtent; }
		VkFormat& GetFormat() { return m_SwapchainFormat; }
		VkSemaphore& GetImageAvailableSemaphore(size_t currentFrame) { return m_ImageAvailableSemaphores[currentFrame]; }
		VkSemaphore& GetRenderFinishedSemaphore(size_t currentFrame) { return m_RenderFinishedSemaphores[currentFrame]; }
		VkFence& GetInFlight(size_t currentFrame) { return m_InFlightFences[currentFrame]; }

		void Resize(size_t width, size_t height);
		void CleanUp();
		void Shutdown(Device& device);
	private:
		void Validate(PhysicalDevice& physicalDevice, Device& device, int32_t width = -1, int32_t height = -1);
		void CreateSyncObject();
		VkSurfaceFormatKHR ChooseSwapchainFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapchainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		VkSwapchainKHR m_Swapchain;
		std::vector<VkImage> m_SwapchainImages;
		std::vector<VkImageView> m_SwapchainViews;
		VkFormat m_SwapchainFormat;
		VkExtent2D m_SwapchainExtent;
		PreFrameInFlight<VkSemaphore> m_ImageAvailableSemaphores;
		PreFrameInFlight<VkSemaphore> m_RenderFinishedSemaphores;
		PreFrameInFlight<VkFence> m_InFlightFences;
	};
}