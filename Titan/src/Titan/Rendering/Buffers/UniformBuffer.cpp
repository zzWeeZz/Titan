#include "TNpch.h"
#include "UniformBuffer.h"
#include "Titan/Rendering/GraphicsContext.h"
#include "Titan/Utils/TitanAllocator.h"
namespace Titan
{
	UniformBuffer::UniformBuffer(const UniformBufferInfo& info)
	{
		VkDeviceSize bufferSize = info.sizeOfData;

		for (size_t i = 0; i < g_FramesInFlight; i++)
		{
			VkBufferCreateInfo bufferInfo{};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = bufferSize;
			bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			VmaAllocationCreateInfo allocInfo{};
			allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

			TitanAllocator::Allocate(m_Buffer[i], &bufferInfo, &allocInfo);
		}
	}
	void UniformBuffer::SetData(void* data, size_t sizeOfData)
	{
		void* mappedData;
		TitanAllocator::MapMemory(m_Buffer[GraphicsContext::GetCurrentFrame()], mappedData);
		memcpy_s(mappedData, sizeOfData, data, sizeOfData);
		TitanAllocator::UnMapMemory(m_Buffer[GraphicsContext::GetCurrentFrame()]);
	}
	Ref<UniformBuffer> UniformBuffer::Create(const UniformBufferInfo& info)
	{
		return CreateRef<UniformBuffer>(info);
	}
}