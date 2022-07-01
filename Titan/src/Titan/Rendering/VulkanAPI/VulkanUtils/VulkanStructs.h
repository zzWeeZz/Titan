#pragma once
#include <deque>
#include <functional>
#include <vulkan/vulkan_core.h>

VkImageCreateInfo ImageCreateInfo(VkFormat format, VkImageUsageFlags flags, VkExtent3D extent); 


struct UploadContext
{
	VkFence uploadFence;
	VkCommandPool commandPool;
	VkCommandBuffer commandBuffer;
};

struct DeletionQueue
{
	std::deque<std::function<void()>> deletors;
	void PushFunction(std::function<void()>&& func)
	{
		deletors.push_back(func);
	}

	void Flush()
	{
		for (auto it = deletors.begin(); it != deletors.end(); ++it)
		{
			(*it)();
		}
		deletors.clear();
	}
};
