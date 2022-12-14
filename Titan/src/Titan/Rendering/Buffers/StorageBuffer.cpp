#include "TNpch.h"
#include "StorageBuffer.h"

#include "Titan/Rendering/GraphicsContext.h"
namespace Titan
{
	StorageBuffer::StorageBuffer(const StorageBufferInfo& info)
	{
		const size_t bufferSize = info.size * info.stride;
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = bufferSize;
		bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
		AllocatedBuffer cpuBuffer;

		TitanAllocator::Allocate(cpuBuffer, &bufferInfo, &allocInfo);

		void* mappedData = nullptr;
		TitanAllocator::MapMemory(cpuBuffer, mappedData);
		memcpy_s(mappedData, bufferSize, info.data, bufferSize);
		TitanAllocator::UnMapMemory(cpuBuffer);

		VkBufferCreateInfo storageBufferInfo{};
		storageBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		storageBufferInfo.pNext = nullptr;

		storageBufferInfo.size = bufferSize;
		storageBufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		TitanAllocator::Allocate(m_GpuBuffer, &storageBufferInfo, &allocInfo);

		GraphicsContext::GetDevice().ImmediateSubmit([=](VkCommandBuffer cmd)
			{
				VkBufferCopy copy{};
				copy.dstOffset = 0;
				copy.srcOffset = 0;
				copy.size = bufferSize;
				vkCmdCopyBuffer(cmd, cpuBuffer.buffer, m_GpuBuffer.buffer, 1, &copy);
			});
		TitanAllocator::DeAllocate(cpuBuffer);
	}

	Ref<StorageBuffer> StorageBuffer::Create(const StorageBufferInfo& info)
	{
		return CreateRef<StorageBuffer>(info);
	}

	StorageBuffer::~StorageBuffer()
	{
	}
}