#include "CommandBuffer.h"
#include "GraphicsContext.h"
#include "Titan/Core/Core.h"

namespace Titan
{
	CommandBuffer::CommandBuffer(VkQueue& queue, uint32_t familyIndex)
	{
		VkCommandPoolCreateInfo poolInfo = CommandPoolCreateInfo(familyIndex, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
		
		TN_VK_CHECK(vkCreateCommandPool(GraphicsContext::GetDevice(), &poolInfo, nullptr, &m_CommandPool));

		VkCommandBufferAllocateInfo allocInfo = CommandBufferAllocateInfo(m_CommandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);

		TN_VK_CHECK(vkAllocateCommandBuffers(GraphicsContext::GetDevice(), &allocInfo, &m_CommandBuffer));
		GlobalDeletionQueue.PushFunction([=]
			{
				vkDestroyCommandPool(GraphicsContext::GetDevice(), m_CommandPool, nullptr);
			});
	}

	void CommandBuffer::Reset()
	{
		TN_VK_CHECK(vkResetCommandBuffer(m_CommandBuffer, 0));
	}

	void CommandBuffer::Bind()
	{
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		TN_VK_CHECK(vkBeginCommandBuffer(m_CommandBuffer, &beginInfo));
	}

	void CommandBuffer::UnBind()
	{

	}

	void CommandBuffer::Shutdown()
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

	VkCommandBufferAllocateInfo CommandBuffer::CommandBufferAllocateInfo(VkCommandPool& commandPool, VkCommandBufferLevel level, uint32_t count)
	{
		VkCommandBufferAllocateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		info.pNext = nullptr;

		info.commandBufferCount = count;
		info.commandPool = commandPool;
		info.level = level;

		return info;
	}
}
