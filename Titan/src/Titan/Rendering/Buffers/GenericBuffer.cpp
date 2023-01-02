#include "TNpch.h"
#include "GenericBuffer.h"

#include "Titan/Rendering/GraphicsContext.h"
namespace Titan
{
	GenericBuffer::GenericBuffer(const GenericBufferInfo& info)
	{
		const size_t bufferSize = info.size * info.stride;
		m_UseFramesInFlight = info.perFrameInFlight;

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.pNext = nullptr;
		bufferInfo.usage = info.usage;
		bufferInfo.size = bufferSize;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = info.allocUsage;

		if (m_UseFramesInFlight)
		{
			for (size_t i = 0; i < g_FramesInFlight; i++)
			{
				bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
				TitanAllocator::Allocate(m_AllocatedBuffers[i], &bufferInfo, &allocInfo);
				if (info.data)
				{
					void* mappedData = nullptr;
					TitanAllocator::MapMemory(m_AllocatedBuffers[i], mappedData);
					memcpy_s(mappedData, bufferSize, info.data, bufferSize);
					TitanAllocator::UnMapMemory(m_AllocatedBuffers[i]);
				}
			}
		}
		else
		{
			TitanAllocator::Allocate(m_AllocatedBuffers[0], &bufferInfo, &allocInfo);
			if (info.data)
			{
				void* mappedData = nullptr;
				TitanAllocator::MapMemory(m_AllocatedBuffers[0], mappedData);
				memcpy_s(mappedData, bufferSize, info.data, bufferSize);
				TitanAllocator::UnMapMemory(m_AllocatedBuffers[0]);
			}
		}
	}


	AllocatedBuffer& GenericBuffer::GetAllocation()
	{
		if (m_UseFramesInFlight)
		{
			return m_AllocatedBuffers[GraphicsContext::GetCurrentFrame()];
		}
		else
		{
			return m_AllocatedBuffers[0];
		}
	}

	PerFrameInFlight<AllocatedBuffer> GenericBuffer::GetRawAllocations()
	{
		return m_AllocatedBuffers;
	}


	Ref<GenericBuffer> GenericBuffer::Create(const GenericBufferInfo& info)
	{
		return CreateRef<GenericBuffer>(info);
	}
}