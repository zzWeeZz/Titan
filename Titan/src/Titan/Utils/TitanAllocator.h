#pragma once

#include "vma/vk_mem_alloc.h"
#include <deque>
#include <functional>

#define TN_ALLOCATOR_TRACE 0
#define TN_ALLOCATOR_TRACK_RAM 0

namespace Titan
{
	struct AllocatedBuffer
	{
		uint32_t id;
		VkBuffer buffer;
		VmaAllocation allocation;
		VkDeviceSize sizeOfBuffer;
	};

	struct AllocatedImage
	{
		uint32_t id;
		VkImage Image;
		VmaAllocation allocation;
		VkDeviceSize sizeOfBuffer;
	};

	class TitanAllocator
	{
	public:
		static void Initialize();
		static void Allocate(AllocatedBuffer& allocation, VkBufferCreateInfo* bufferInfo, VmaAllocationCreateInfo* allocationInfo);
		static void Allocate(AllocatedImage& allocation, VkImageCreateInfo* imageInfo, VmaAllocationCreateInfo* allocationInfo);
		static void DeAllocate(AllocatedBuffer& allocation);
		static void DeAllocate(AllocatedImage& allocation);

		static void MapMemory(AllocatedBuffer& allocation, void*& mappedMemory);
		static void UnMapMemory(AllocatedBuffer& allocation);
		static void QueueDeletion(std::function<void()>&& func);
		static void Flush();
		static void Shutdown();

#if TN_ALLOCATOR_TRACK_RAM
		static void TrackRamAlloc(const size_t& size);
		static void TrackRamDealloc(const size_t& size);
#endif

	private:
		inline static VmaAllocator s_Allocator;
		inline static uint32_t s_ID;
		inline static std::unordered_map<uint32_t, std::function<void()>> s_DestroyFunctions;
		inline static std::vector<uint32_t> s_AllocateDestructorOrder;
		inline static std::deque<std::function<void()>> s_DestructionQueue;
	};
}

#if TN_ALLOCATOR_TRACK_RAM

#pragma warning(disable:28196)
#pragma warning(disable:28251)
#pragma warning(disable:6387)

void* operator new(size_t size)
{
#if TN_ALLOCATOR_TRACE
	TN_CORE_INFO("TitanAllocator: Allocating ram memory: {0} bytes", size);
#endif
	Titan::TitanAllocator::TrackRamAlloc(size);
	return malloc(size);
}

void operator delete(void* addess, size_t size)
{
#if TN_ALLOCATOR_TRACE
	TN_CORE_INFO("TitanAllocator: Deallocating ram memory: {0} bytes", size);
#endif
	Titan::TitanAllocator::TrackRamDealloc(size);
	free(addess);
}

#pragma warning(default:28196)
#pragma warning(default:28251)
#pragma warning(default:6387)

#endif // TN_ALLOCATOR_TRACK_RAM