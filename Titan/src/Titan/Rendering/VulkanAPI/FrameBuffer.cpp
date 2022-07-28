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
		VkExtent3D imageExtent;
		imageExtent.width = spec.Width;
		imageExtent.height = spec.Height;
		imageExtent.depth = 1;
		auto& device = GraphicsContext::GetDevice();

		const uint32_t colorAttachmentCount = 3;
		m_FrameBuffers = std::vector<VkFramebuffer>(colorAttachmentCount);
		m_AllocatedImages.resize(colorAttachmentCount);
		m_Views.resize(colorAttachmentCount);
		for (uint32_t i = 0; i < colorAttachmentCount; i++)
		{
			VkImageView attachments[2];
			if (!spec.isSwapchain)
			{
				VkImageCreateInfo CreateImageInfo{};
				CreateImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
				CreateImageInfo.pNext = nullptr;
				CreateImageInfo.imageType = VK_IMAGE_TYPE_2D;
				CreateImageInfo.format = VK_FORMAT_B8G8R8A8_SRGB;
				CreateImageInfo.extent = imageExtent;
				CreateImageInfo.mipLevels = 1;
				CreateImageInfo.arrayLayers = 1;
				CreateImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
				CreateImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
				CreateImageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
				CreateImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

				VmaAllocationCreateInfo allocInfo{};
				allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

				TN_VK_CHECK(vmaCreateImage(GraphicsContext::GetAllocator(), &CreateImageInfo, &allocInfo, &m_AllocatedImages[i].Image, &m_AllocatedImages[i].Allocation, nullptr));

				VkImageViewCreateInfo ViewInfo{};
				ViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				ViewInfo.pNext = nullptr;

				ViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
				ViewInfo.image = m_AllocatedImages[i].Image;
				ViewInfo.format = VK_FORMAT_B8G8R8A8_SRGB;
				ViewInfo.subresourceRange.baseMipLevel = 0;
				ViewInfo.subresourceRange.levelCount = 1;
				ViewInfo.subresourceRange.baseArrayLayer = 0;
				ViewInfo.subresourceRange.layerCount = 1;
				ViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				vkCreateImageView(GraphicsContext::GetDevice(), &ViewInfo, nullptr, &m_Views[i]);

				attachments[0] = m_Views[i];
			}
			else
			{
				attachments[0] = spec.Views[i];
			}
			attachments[1] = spec.DepthViews;
			framebufferInfo.pAttachments = &attachments[0];
			framebufferInfo.attachmentCount = 2;
			TN_VK_CHECK(vkCreateFramebuffer(GraphicsContext::GetDevice(), &framebufferInfo, nullptr, &m_FrameBuffers[i]));
			GlobalDeletionQueue.PushFunction([=]
				{
					if (!spec.isSwapchain)
					{

						vkDestroyImageView(device, m_Views[i], nullptr);
						vmaDestroyImage(GraphicsContext::GetAllocator(), m_AllocatedImages[i].Image, m_AllocatedImages[i].Allocation);
					}
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
