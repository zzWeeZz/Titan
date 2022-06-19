#include "FrameBuffer.h"
#include "GraphicsContext.h"

namespace Titan
{
	FrameBuffer::FrameBuffer(const FrameBufferInfo& spec)
	{
		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = spec.RenderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.width = spec.Width;
		framebufferInfo.height = spec.Height;
		framebufferInfo.layers = 1;

		const uint32_t colorAttachmentCount = GraphicsContext::GetSwapChain().GetImages().size();
		m_FrameBuffers = std::vector<VkFramebuffer>(colorAttachmentCount);

		for (uint32_t i = 0; i < colorAttachmentCount; i++)
		{
			framebufferInfo.pAttachments = &GraphicsContext::GetSwapChain().GetViews()[i];
			TN_VK_CHECK(vkCreateFramebuffer(GraphicsContext::GetDevice(), &framebufferInfo, nullptr, &m_FrameBuffers[i]));
		}
	}

	void FrameBuffer::Shutdown()
	{
		for (auto mFrameBuffer : m_FrameBuffers)
		{
			vkDestroyFramebuffer(GraphicsContext::GetDevice(), mFrameBuffer, nullptr);
		}
	}

	Ref<FrameBuffer> FrameBuffer::Create(const FrameBufferInfo& spec)
	{
		return CreateRef<FrameBuffer>(spec);
	}
}
