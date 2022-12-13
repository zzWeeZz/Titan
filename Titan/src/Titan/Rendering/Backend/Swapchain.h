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
		Swapchain();
		~Swapchain() = default;

		// Setups the syncstructure, creates renderpass and creates vkFramebuffers per swapchain image.
		//OBS: Titan::PhysicalDevice and Titan::Device needs to be created before calling this function! 
		void Create(PhysicalDevice& physicalDevice, Device& device);
		const int32_t GetNextImage();
		const uint32_t GetCurrentImageIndex();
		VkImage& GetImage(size_t index);
		VkImageView& GetCurrentView(size_t index);
		VkSwapchainKHR& GetHandle() { return m_Swapchain; }
		VkExtent2D& GetExtent() { return m_SwapchainExtent; }
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
		// Creates the swapchain object with a width and height.
		// 
		// HOW IT WORKS:
		// It Fetches all swapchain data from the GraphicsContext, pysical device, and device.
		// After fetching it starts to fretch queue families for the swapchain create info struct from vulkan.
		// Now Creates the vkSwapchain object.
		// Creates all swapchain images.
		// OBS: If a swapchain or its images are still alive in memory they need to be destroyed before calling this function.
		void Validate(PhysicalDevice& physicalDevice, Device& device, int32_t width = -1, int32_t height = -1);

		// Resizes the swapchain after width and height.
		// 
		// HOW IT WORKS:
		// Cleans up the swapchain by deallocating swapchain and its images.
		// Calls Validate to recreate the swapchain.
		void InternalResize(size_t width, size_t height);
		void CreateSyncObject();

		// Creates a basic VkRenderpass connected to the swapchain.
		// OBS: This renderpass is only used by ImGui!
		void CreateRenderPass();

		// Creates basic Vkframebuffers per swapchain image.
		// OBS: These Vkframebuffers is only used by ImGui!
		void CreateFrameBuffer();

		// Tires to get teh optimal swapchain format for the current gpu.
		VkSurfaceFormatKHR ChooseSwapchainFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

		// Tries to get the optimal present mode for the current gpu.
		VkPresentModeKHR ChooseSwapchainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

		// Builds a vkExtent after the screen size.
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