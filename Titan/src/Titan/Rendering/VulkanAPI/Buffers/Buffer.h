#pragma once
#include "Buffers.h"
#include "Titan/Core/Core.h"


namespace Titan
{
	class Buffer
	{
	public:
		Buffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
		VmaAllocation& GetAllocation() { return m_Buffer.Allocation; }
		VkBuffer& GetBuffer() { return m_Buffer.Buffer; }
		void Destroy() const;
		static Ref<Buffer> Create(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
	private:
		AllocatedBuffer m_Buffer;
	};
}
