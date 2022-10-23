#pragma once
#include "vma/vk_mem_alloc.h"
#include <deque>
#include <functional>
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
		VkImage Image;
		VmaAllocation allocation;
	};

	class TitanAllocator
	{
	public:
		static void Initialize();
		static void Allocate(AllocatedBuffer& allocation, VkBufferCreateInfo* bufferInfo, VmaAllocationCreateInfo* allocationInfo);
		static void Allocate(AllocatedImage& allocation);
		static void DeAllocate(AllocatedBuffer& allocation);
		static void DeAllocate(AllocatedImage& allocation);

		static void MapMemory(AllocatedBuffer& allocation, void*& mappedMemory);
		static void UnMapMemory(AllocatedBuffer& allocation);
		static void QueueDeletion(std::function<void()>&& func);
		static void Flush();
		static void Shutdown();
	private:
		inline static VmaAllocator s_Allocator;
		inline static uint32_t s_ID;
		inline static std::unordered_map<uint32_t, std::function<void()>> s_DestroyFunctions;
		inline static std::vector<uint32_t> s_AllocateDestructorOrder;
		inline static std::deque<std::function<void()>> s_DestructionQueue;
	};
}