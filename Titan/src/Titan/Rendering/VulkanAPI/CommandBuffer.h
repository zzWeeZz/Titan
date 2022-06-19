#pragma once
#include <vulkan/vulkan.h>
#include "Titan/Core/Core.h"

namespace Titan
{
	class CommandBuffer
	{
	public:
		explicit CommandBuffer(VkQueue& queue, uint32_t familyIndex);
		void Reset();

		VkCommandBuffer& GetHandle() { return m_CommandBuffer; }

		void Bind();
		void UnBind();

		void Shutdown();
		static Ref<CommandBuffer> Create(VkQueue& queue, uint32_t familyIndex);
	private:
		inline static VkCommandPoolCreateInfo CommandPoolCreateInfo(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags);
		inline static VkCommandBufferAllocateInfo CommandBufferAllocateInfo(VkCommandPool& commandPool, VkCommandBufferLevel level, uint32_t count);

		VkCommandBuffer m_CommandBuffer;
		VkCommandPool m_CommandPool;
	};
}
