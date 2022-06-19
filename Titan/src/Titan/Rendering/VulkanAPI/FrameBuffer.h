#pragma once
#include "Titan/Core/Core.h"
#include <vulkan/vulkan.h>

namespace Titan
{
	struct FrameBufferInfo
	{
		uint32_t Width;
		uint32_t Height;
	};

	class FrameBuffer
	{
	public:
		FrameBuffer();

		static Ref<FrameBuffer> Create(const FrameBufferInfo& spec);
	private:
		VkFramebuffer m_FrameBuffer;
	};
}