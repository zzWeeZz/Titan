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
			/*VkImageCreateInfo depthImgInfo{};
			depthImgInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			depthImgInfo.pNext = nullptr;
			depthImgInfo.imageType = VK_IMAGE_TYPE_2D;
			depthImgInfo.format = VK_FORMAT_B8G8R8A8_SRGB;
			depthImgInfo.extent = depthImageExtent;
			depthImgInfo.mipLevels = 1;
			depthImgInfo.arrayLayers = 1;
			depthImgInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			depthImgInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			depthImgInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;


			VmaAllocationCreateInfo dimAlloc{};
			dimAlloc.usage = VMA_MEMORY_USAGE_GPU_ONLY;
			dimAlloc.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			TN_VK_CHECK(vmaCreateImage(GraphicsContext::GetAllocator(), &depthImgInfo, &dimAlloc, &m_AllocatedImages[i].Image, &m_AllocatedImages[i].Allocation, nullptr));
			
			VkImageViewCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			info.pNext = nullptr;
			info.format = VK_FORMAT_B8G8R8A8_SRGB;
			info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			info.image = m_AllocatedImages[i].Image;
			info.subresourceRange.baseMipLevel = 0;
			info.subresourceRange.levelCount = 1;
			info.subresourceRange.baseArrayLayer = 0;
			info.subresourceRange.layerCount = 1;
			info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

			TN_VK_CHECK(vkCreateImageView(device, &info, nullptr, &m_Views[i]));*/


			framebufferInfo.pAttachments = &GraphicsContext::GetSwapChain().GetViews()[i];
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
