#include "Buffer.h"

#include "Titan/Rendering/VulkanAPI/GraphicsContext.h"

namespace Titan
{
	Buffer::Buffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage)
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.pNext = nullptr;

		bufferInfo.size = allocSize;
		bufferInfo.usage = usage;

		VmaAllocationCreateInfo vmaAllocInfo{};
		vmaAllocInfo.usage = memoryUsage;

		TN_VK_CHECK(vmaCreateBuffer(GraphicsContext::GetAllocator(), &bufferInfo, &vmaAllocInfo, &m_Buffer.Buffer, &m_Buffer.Allocation, nullptr));
	}

	void Buffer::Bind(void* data, size_t size)
	{
		void* mem;
		vmaMapMemory(GraphicsContext::GetAllocator(), m_Buffer.Allocation, &mem);
		memcpy(mem, data, size);
		vmaUnmapMemory(GraphicsContext::GetAllocator(), m_Buffer.Allocation);
	}

	void Buffer::Destroy() const
	{
		vmaDestroyBuffer(GraphicsContext::GetAllocator(), m_Buffer.Buffer, m_Buffer.Allocation);
	}

	Ref<Buffer> Buffer::Create(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage)
	{
		return CreateRef<Buffer>(allocSize, usage, memoryUsage);
	}
}
