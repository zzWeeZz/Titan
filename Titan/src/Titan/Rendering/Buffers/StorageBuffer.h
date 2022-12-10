#pragma once
#include "Titan/Utils/TitanAllocator.h"

#include "Titan/Core/TitanMemory.h"
namespace Titan
{
	struct StorageBufferInfo
	{
		void* data;
		size_t size;
		size_t stride;
	};

	class StorageBuffer
	{
	public:
		StorageBuffer(const StorageBufferInfo& info);
		AllocatedBuffer& GetAllocation() { return m_GpuBuffer; }
		static Ref<StorageBuffer> Create(const StorageBufferInfo& info);
		~StorageBuffer();
	private:
		AllocatedBuffer m_GpuBuffer;
	};
}