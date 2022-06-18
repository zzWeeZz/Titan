#include "CommandBuffer.h"

namespace Titan
{
	CommandBuffer::CommandBuffer(VkQueue& queue, uint32_t familyIndex)
	{

	}

	Ref<CommandBuffer> CommandBuffer::Create(VkQueue& queue, uint32_t familyIndex)
	{
		return CreateRef<CommandBuffer>(queue, familyIndex);
	}

	VkCommandPoolCreateInfo CommandBuffer::CommandPoolCreateInfo(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags)
	{
		VkCommandPoolCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		info.pNext = nullptr;

		info.queueFamilyIndex = queueFamilyIndex;
		info.flags = flags;
		return info;
	}
	
}
