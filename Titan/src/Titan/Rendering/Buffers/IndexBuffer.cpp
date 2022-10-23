#include "TNpch.h"
#include "IndexBuffer.h"
#include "Titan/Rendering/GraphicsContext.h"
namespace Titan
{
	IndexBuffer::IndexBuffer(const IndexBufferInfo& info)
	{
		size_t bufferSize = info.sizeOfArray * sizeof(uint32_t);
		m_IndexCount = info.sizeOfArray;
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
		memcpy_s(mappedData, bufferSize, info.indexData, bufferSize);
		TitanAllocator::UnMapMemory(cpuBuffer);

		VkBufferCreateInfo vertexBufferInfo{};
		vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		vertexBufferInfo.pNext = nullptr;

		vertexBufferInfo.size = bufferSize;
		vertexBufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		TitanAllocator::Allocate(m_GpuBuffer, &vertexBufferInfo, &allocInfo);

		GraphicsContext::GetDevice().ImmediateSubmit([=](VkCommandBuffer cmd)
			{
				VkBufferCopy copy;
				copy.dstOffset = 0;
				copy.srcOffset = 0;
				copy.size = bufferSize;
				vkCmdCopyBuffer(cmd, cpuBuffer.buffer, m_GpuBuffer.buffer, 1, &copy);
			});

		TitanAllocator::DeAllocate(cpuBuffer);
	}
	
	size_t IndexBuffer::GetIndexCount()
	{
		return	m_IndexCount;
	}
	Ref<IndexBuffer> IndexBuffer::Create(const IndexBufferInfo& info)
	{
		return CreateRef<IndexBuffer>(info);
	}
	IndexBuffer::~IndexBuffer()
	{
		
	}
}