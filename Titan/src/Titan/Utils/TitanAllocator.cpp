#include "TNpch.h"
#include "TitanAllocator.h"
#include "Titan/Rendering/GraphicsContext.h"
namespace Titan
{
	void TitanAllocator::Initialize()
	{
		VmaAllocatorCreateInfo createInfo{};
		createInfo.device = GraphicsContext::GetDevice().GetHandle();
		createInfo.instance = GraphicsContext::GetInstance();
		createInfo.physicalDevice = GraphicsContext::GetPhysicalDevice().GetHandle();
		s_ID = 0;
		TN_VK_CHECK(vmaCreateAllocator(&createInfo, &s_Allocator));
	}

	void TitanAllocator::Allocate(AllocatedBuffer& allocation, VkBufferCreateInfo* bufferInfo, VmaAllocationCreateInfo* allocationInfo)
	{
		allocation.id = s_ID;
		TN_CORE_INFO("TitanAllocator: id {0} Allocating buffer: {1} bytes.", allocation.id, bufferInfo->size);
		allocation.sizeOfBuffer = bufferInfo->size;
		TN_VK_CHECK(vmaCreateBuffer(s_Allocator, bufferInfo, allocationInfo, &allocation.buffer, &allocation.allocation, nullptr));
		s_AllocateDestructorOrder.push_back(allocation.id);
		s_DestroyFunctions[allocation.id] = [&, allocation]() {TN_CORE_INFO("TitanAllocator: id {0} Deallocating buffer: {1} bytes", allocation.id, allocation.sizeOfBuffer); vmaDestroyBuffer(s_Allocator, allocation.buffer, allocation.allocation); };
		s_ID++;
	}

	void TitanAllocator::Allocate(AllocatedImage& allocation, VkImageCreateInfo* imageInfo, VmaAllocationCreateInfo* allocationInfo)
	{
		allocation.id = s_ID;
		TN_VK_CHECK(vmaCreateImage(s_Allocator, imageInfo, allocationInfo, &allocation.Image, &allocation.allocation, nullptr));
		VmaAllocationInfo allocInfo{};
		vmaGetAllocationInfo(s_Allocator, allocation.allocation, &allocInfo);
		allocation.sizeOfBuffer = allocInfo.size;
		s_AllocateDestructorOrder.push_back(allocation.id);
		TN_CORE_INFO("TitanAllocator: id {0} Allocating image: {1} bytes.", allocation.id, allocation.sizeOfBuffer);
		s_DestroyFunctions[allocation.id] = [&, allocation]() {TN_CORE_INFO("TitanAllocator: id {0} Deallocating image: {1} bytes", allocation.id, allocation.sizeOfBuffer); vmaDestroyImage(s_Allocator, allocation.Image, allocation.allocation); };
		s_ID++;
	}

	void TitanAllocator::DeAllocate(AllocatedBuffer& allocation)
	{
		TN_CORE_INFO("TitanAllocator: id {0} Deallocating buffer: {1} bytes", allocation.id, allocation.sizeOfBuffer);
		vmaDestroyBuffer(s_Allocator, allocation.buffer, allocation.allocation);
		s_DestroyFunctions.erase(allocation.id);
		auto it = std::find(s_AllocateDestructorOrder.begin(), s_AllocateDestructorOrder.end(), allocation.id);
		if (it != s_AllocateDestructorOrder.end())
		{
			s_AllocateDestructorOrder.erase(it);
		}
	}

	void TitanAllocator::DeAllocate(AllocatedImage& allocation)
	{
		TN_CORE_INFO("TitanAllocator: id {0} Deallocating image: {1} bytes", allocation.id, allocation.sizeOfBuffer);
		vmaDestroyImage(s_Allocator, allocation.Image, allocation.allocation);
		s_DestroyFunctions.erase(allocation.id);
		auto it = std::find(s_AllocateDestructorOrder.begin(), s_AllocateDestructorOrder.end(), allocation.id);
		if (it != s_AllocateDestructorOrder.end())
		{
			s_AllocateDestructorOrder.erase(it);
		}
	}

	void TitanAllocator::MapMemory(AllocatedBuffer& allocation, void*& mappedMemory)
	{
		TN_VK_CHECK(vmaMapMemory(s_Allocator, allocation.allocation, &mappedMemory));
	}

	void TitanAllocator::UnMapMemory(AllocatedBuffer& allocation)
	{
		vmaUnmapMemory(s_Allocator, allocation.allocation);
	}

	void TitanAllocator::QueueDeletion(std::function<void()>&& func)
	{
		s_DestructionQueue.push_back(func);
	}

	void TitanAllocator::Flush()
	{
		for (int32_t Index = s_AllocateDestructorOrder.size() - 1; Index >= 0 ; Index--)
		{
			s_DestroyFunctions[s_AllocateDestructorOrder[Index]]();
		}
		for (auto it = s_DestructionQueue.begin(); it != s_DestructionQueue.end(); ++it)
		{
			(*it)();
		}
		s_DestructionQueue.clear();
	}
	void TitanAllocator::Shutdown()
	{
		vmaDestroyAllocator(s_Allocator);
	}
}