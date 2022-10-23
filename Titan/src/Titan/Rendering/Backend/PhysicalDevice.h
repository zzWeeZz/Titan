#pragma once
#include <vulkan/vulkan.h>
#include <optional>
namespace Titan
{
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool HasValue()
		{
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};
	class PhysicalDevice
	{
	public:
		PhysicalDevice() = default;
		~PhysicalDevice() = default;

		VkPhysicalDevice& GetHandle() { return m_PhysicalDevice; }
		void Create(VkInstance& instance);
		QueueFamilyIndices FindQueueFamilies();
	private:
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
	};
}