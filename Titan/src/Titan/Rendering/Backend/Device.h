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
		VkCommandBuffer& GetCommandBuffer(size_t currentFrame, size_t index) { return m_CommandBuffers[index][currentFrame]; }
		VkCommandPool& GetCommandPool(size_t index) { return m_CommandPools[index]; }
		VkQueue& GetGraphicsQueue() { return m_GraphicsQueue; }
		VkQueue& GetPresentQueue() { return m_PresentQueue; }
		void WaitForIdle();
		VkCommandBuffer CreateSecondaryCommandBuffer();
		void ImmediateSubmit(std::function<void(VkCommandBuffer cmd)>&& func);
		void Shutdown();
	private:
		void CreateCommandPools(PhysicalDevice& physicalDevice, size_t amount = 1);
		bool CheckDeviceExtensionSupport(PhysicalDevice& physicalDevice, const std::vector<const char*> deviceExtensions);
		VkDevice m_Device{};
		VkQueue m_GraphicsQueue{};
		VkQueue m_PresentQueue{};
		std::vector<PerFrameInFlight<VkCommandBuffer>> m_CommandBuffers;
		std::vector<VkCommandPool> m_CommandPools;
	};
}