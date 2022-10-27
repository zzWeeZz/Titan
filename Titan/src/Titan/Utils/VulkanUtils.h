#pragma once
#include <vulkan/vulkan.h>
#include "Titan/Utils/TitanAllocator.h"
namespace Titan
{
	class VulkanUtils
	{
	public:
		static void MemoryBarrier(VkCommandBuffer cmd, AllocatedImage& image);
	};
}