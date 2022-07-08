#include "FrameBuffer.h"
#include "GraphicsContext.h"
#include "Titan/Application.h"
#include "vk_mem_alloc.h"

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

		auto& device = GraphicsContext::GetDevice();

		const uint32_t colorAttachmentCount = GraphicsContext::GetSwapChain().GetImages().size();
		m_FrameBuffers = std::vector<VkFramebuffer>(colorAttachmentCount);
		/*m_Views.resize(colorAttachmentCount);
		m_Images.resize(colorAttachmentCount);
		m_AllocatedImages.resize(colorAttachmentCount);
		VkExtent3D depthImageExtent =
		{
			Application::GetWindow().GetWidth(),
			Application::GetWindow().GetHeight(),
			1
		};*/

		for (uint32_t i = 0; i < colorAttachmentCount; i++)
		{
			VkImageView attachments[2];
			attachments[0] = GraphicsContext::GetSwapChain().GetViews()[i];
			attachments[1] = GraphicsContext::GetSwapChain().GetDepthView();
			framebufferInfo.pAttachments = &attachments[0];
			framebufferInfo.attachmentCount = 2;
			TN_VK_CHECK(vkCreateFramebuffer(GraphicsContext::GetDevice(), &framebufferInfo, nullptr, &m_FrameBuffers[i]));
			GlobalDeletionQueue.PushFunction([=]
				{
					vkDestroyImageView(device, GraphicsContext::GetSwapChain().GetViews()[i], nullptr);
					vkDestroyFramebuffer(GraphicsContext::GetDevice(), m_FrameBuffers[i], nullptr);
				});
		}
	
	}

	void FrameBuffer::Shutdown()
	{
		
	}

	Ref<FrameBuffer> FrameBuffer::Create(const FrameBufferInfo& spec)
	{
		return CreateRef<FrameBuffer>(spec);
	}
}
