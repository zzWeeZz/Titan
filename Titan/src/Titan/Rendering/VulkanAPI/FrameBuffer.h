#pragma once
#include "Titan/Core/Core.h"
#include <vulkan/vulkan.h>

namespace Titan
{
	struct FrameBufferInfo
	{
		bool shouldResize;
		uint32_t Width;
		uint32_t Height;
		VkRenderPass RenderPass;
	};

	class FrameBuffer
	{
	public:
		explicit FrameBuffer(const FrameBufferInfo& spec);
		VkFramebuffer& GetFrameBuffer(uint32_t imageIndex) { return m_FrameBuffers[imageIndex]; }
		void Shutdown();
		static Ref<FrameBuffer> Create(const FrameBufferInfo& spec);
	private:
		std::vector<VkFramebuffer> m_FrameBuffers;
	};
}