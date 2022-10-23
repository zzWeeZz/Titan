#include "TNpch.h"
#include "VertexBuffer.h"
#include <vulkan/vulkan.h>
#include "Titan/Rendering/GraphicsContext.h"
namespace Titan
{
    VertexBuffer::VertexBuffer(const VertexBufferInfo& info)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = info.sizeOfVertex * info.sizeOfArray;
        bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
		AllocatedBuffer cpuBuffer;

        TitanAllocator::Allocate(cpuBuffer, &bufferInfo, &allocInfo);

		void* mappedData = nullptr;
		TitanAllocator::MapMemory(cpuBuffer, mappedData);
		memcpy_s(mappedData, info.sizeOfVertex * info.sizeOfArray, info.vertexData, info.sizeOfVertex * info.sizeOfArray);
		TitanAllocator::UnMapMemory(cpuBuffer);

		VkBufferCreateInfo vertexBufferInfo{};
		vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		vertexBufferInfo.pNext = nullptr;

		vertexBufferInfo.size = info.sizeOfVertex * info.sizeOfArray;
		vertexBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        TitanAllocator::Allocate(m_GpuBuffer, &vertexBufferInfo, &allocInfo);

        GraphicsContext::GetDevice().ImmediateSubmit([=](VkCommandBuffer cmd)
            {
                VkBufferCopy copy;
                copy.dstOffset = 0;
                copy.srcOffset = 0;
                copy.size = info.sizeOfVertex * info.sizeOfArray;
                vkCmdCopyBuffer(cmd, cpuBuffer.buffer, m_GpuBuffer.buffer, 1, &copy);
            });

        TitanAllocator::DeAllocate(cpuBuffer);
    }

    Ref<VertexBuffer> VertexBuffer::Create(const VertexBufferInfo& info)
    {
        return CreateRef<VertexBuffer>(info);
    }
}