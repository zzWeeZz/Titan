#pragma once
#include <vulkan/vulkan.h>
#include "Titan/Utils/TitanAllocator.h"
#include "Titan/Rendering/GraphicsContext.h"
namespace Titan
{
	struct UniformBufferInfo
	{
		void* data;
		size_t sizeOfData;
	};
	class UniformBuffer
	{
	public:
		UniformBuffer(const UniformBufferInfo& info);
		void SetData(void* data, size_t sizeOfData);
		AllocatedBuffer& GetAllocation() { return m_Buffer[GraphicsContext::GetCurrentFrame()]; }
		static Ref<UniformBuffer> Create(const UniformBufferInfo& info);
	private:
		PerFrameInFlight<AllocatedBuffer> m_Buffer;
	};
}