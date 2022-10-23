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
		
		//TN_VK_CHECK(vmaCreateAllocator(&createInfo, &s_Allocator));
		//QueueDeletion([&]() {vmaDestroyAllocator(s_Allocator); });
	}

	void TitanAllocator::Allocate(AllocatedBuffer& allocation)
	{
		
	}

	void TitanAllocator::Allocate(AllocatedImage& allocation)
	{
	}

	void TitanAllocator::QueueDeletion(std::function<void()>&& func)
	{
		s_DestructionQueue.push_back(func);
	}

	void TitanAllocator::Flush()
	{
		for (auto it = s_DestructionQueue.begin(); it != s_DestructionQueue.end(); ++it)
		{
			(*it)();
		}
		s_DestructionQueue.clear();
	}
}