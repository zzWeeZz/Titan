#pragma once
#include "Titan/Core/Core.h"
#include <vulkan/vulkan.h>

#include "FrameBuffer.h"

namespace Titan
{
	struct RenderPassCreateInfo
	{
		uint32_t width = 1024;
		uint32_t height = 1024;
		VkFormat colorFormat = VK_FORMAT_B8G8R8A8_SRGB;
		bool haveDepth = true;
		VkFormat depthFormat = VK_FORMAT_D32_SFLOAT;
	};

	class RenderPass
	{
	public:
		RenderPass(const RenderPassCreateInfo& info);
		Ref<FrameBuffer> GetFrameBuffer() { return m_FrameBuffer; }
		VkRenderPass& GetRenderPass() { return m_RenderPass; }
		void Shutdown();
		static Ref<RenderPass> Create(const RenderPassCreateInfo& info);
	private:
		VkRenderPass m_RenderPass;
		Ref<FrameBuffer> m_FrameBuffer;
	};
}
