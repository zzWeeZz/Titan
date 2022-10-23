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
		VkExtent2D GetExtent() { return m_SwapchainExtent; }
		void Shutdown(Device& device);
	private:
		VkSurfaceFormatKHR ChooseSwapchainFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapchainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		VkSwapchainKHR m_Swapchain;
		std::vector<VkImage> m_SwapchainImages;
		std::vector<VkImageView> m_SwapchainViews;
		VkFormat m_SwapchainFormat;
		VkExtent2D m_SwapchainExtent;
	};
}