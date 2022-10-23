#pragma once
#include "vma/vk_mem_alloc.h"
#include <deque>
#include <functional>
namespace Titan
{
	struct AllocatedBuffer
	{
		VkBuffer buffer;
		VmaAllocation allocation;
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
		static void Allocate(AllocatedBuffer& allocation);
		static void Allocate(AllocatedImage& allocation);
		static void QueueDeletion(std::function<void()>&& func);
		static void Flush();
	private:
		inline static VmaAllocator s_Allocator;
		inline static std::deque<std::function<void()>> s_DestructionQueue;
	};
}