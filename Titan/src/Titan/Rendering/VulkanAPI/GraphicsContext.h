#pragma once
#include <vulkan/vulkan.h>

#include "CommandBuffer.h"
#include "Swapchain.h"
#include "Buffers/Buffers.h"
#include "VulkanUtils/VulkanStructs.h"
#include "Titan/Core/Core.h"
#include "Titan/Window.h"
namespace Titan
{
	class GraphicsContext
	{
	public:
		static void Initialize();
		void static CreateCommandBuffer(Ref<CommandBuffer>& outCommandBuffer);
		inline static VkInstance& GetInstance() { return m_Instance; }
		inline static VkDevice& GetDevice() { return m_Device; }
		inline static VkPhysicalDevice& GetPhysicalDevice() { return m_PhysicalDevice; }
		inline static VkSurfaceKHR& GetSurface() { return m_Surface; }
		inline static Swapchain& GetSwapChain() { return *m_Swapchain.get(); }
		inline static VkQueue& GetGraphicsQueue() { return m_GraphicsQueue; }
		inline static VmaAllocator& GetAllocator() { return m_Allocator; }

		static void ImmediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function);

		static void ShutDown();
	private:
		static void CreateInstance();
		static void SetupDebugMessenger();
		static bool CheckValidationLayerSupport(std::vector<const char*>& vector);
		inline static VkDebugUtilsMessengerEXT m_DebugMessenger;

		inline static UploadContext m_UploadContext;

		inline static VkDevice m_Device;
		inline static VkPhysicalDevice m_PhysicalDevice;

		inline static VkInstance m_Instance;
		inline static VkSurfaceKHR m_Surface;

		inline static VkQueue m_GraphicsQueue;
		inline static uint32_t m_GraphicsQueueFamily;

		inline static Ref<Swapchain> m_Swapchain;

		inline static VmaAllocator m_Allocator;
	};
}
