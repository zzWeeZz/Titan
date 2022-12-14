#pragma once
#include <vulkan/vulkan.h>

#include "Titan/Core/TitanMemory.h"

#include "Titan/Utils/TitanAllocator.h"


namespace Titan
{
	struct GenericBufferInfo
	{
		void* data = nullptr;
		size_t size = 0;
		size_t stride = 0;
		VkBufferUsageFlags usage = 0;
		VmaMemoryUsage allocUsage;
		bool perFrameInFlight = true;
	};

	class GenericBuffer
	{
	public:
		GenericBuffer(const GenericBufferInfo& info);
		AllocatedBuffer& GetAllocation();
		static Ref<GenericBuffer> Create(const GenericBufferInfo& info);
	private:
		bool m_UseFramesInFlight; 
		PerFrameInFlight<AllocatedBuffer> m_AllocatedBuffers;
	};
}