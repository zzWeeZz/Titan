#pragma once
#include "Titan/Utils/TitanAllocator.h"
namespace Titan
{
	class VulkanUtils
	{
	public:
		static void MemoryBarrier(VkCommandBuffer cmd, AllocatedImage& image, VkPipelineStageFlagBits src, VkPipelineStageFlagBits dst);
	};
}