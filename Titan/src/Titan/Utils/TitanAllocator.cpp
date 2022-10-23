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
		
		TN_VK_CHECK(vmaCreateAllocator(&createInfo, &s_Allocator));
	}
	void TitanAllocator::Allocate(AllocatedBuffer& allocation)
	{
		
	}
	void TitanAllocator::Allocate(AllocatedImage& allocation)
	{
	}
}