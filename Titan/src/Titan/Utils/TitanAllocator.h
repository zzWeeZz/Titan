#pragma once
#include "vma/vk_mem_alloc.h"
#include <deque>
namespace Titan
{
	struct DeletionQueue
	{
		std::deque<std::function<void()>> deletors;
		void PushFunction(std::function<void()>&& func)
		{
			deletors.push_back(func);
		}

		void Flush()
		{
			for (auto it = deletors.begin(); it != deletors.end(); ++it)
			{
				(*it)();
			}
			deletors.clear();
		}
	};

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
		inline static DeletionQueue DestructionQueue;
	private:
		inline static VmaAllocator s_Allocator;
	};
}