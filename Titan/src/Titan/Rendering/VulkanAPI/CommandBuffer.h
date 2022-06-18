#pragma once
#include <vulkan/vulkan.h>

#include "Titan/Core/Core.h"

namespace Titan
{
	class CommandBuffer
	{
	public:
		explicit CommandBuffer(VkQueue& queue, uint32_t familyIndex);

		static Ref<CommandBuffer> Create(VkQueue& queue, uint32_t familyIndex);
	private:
		inline static VkCommandPoolCreateInfo CommandPoolCreateInfo(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags);

		VkCommandBuffer m_CommandBuffer;
		VkCommandPool m_CommandPool;
	};
}
