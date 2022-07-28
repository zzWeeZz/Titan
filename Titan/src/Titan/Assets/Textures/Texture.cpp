#include "Texture.h"

#include "Titan/AssetImporter/AssetImporter.h"
#include "Titan/Rendering/VulkanAPI/CommandBuffer.h"
#include "Titan/Rendering/VulkanAPI/GraphicsContext.h"
#include "Titan/Rendering/VulkanAPI/Buffers/Buffer.h"

namespace Titan
{
	Texture::Texture(const std::filesystem::path& path)
	{
		int texWidth, texHeight, texChannels;
		m_Pixels = stbi_load(path.string().c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

		if (!m_Pixels)
		{
			TN_CORE_ERROR("Failed to load texture file: {0}", path.string().c_str());
			return;
		}
		void* pixel_ptr = m_Pixels;
		VkDeviceSize imageSize = texWidth * texHeight * 4;

		VkFormat image_format = VK_FORMAT_R8G8B8A8_SRGB;

		const Ref<Buffer> buffer = Buffer::Create(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

		buffer->Bind(m_Pixels, imageSize);
		
		stbi_image_free(m_Pixels);

		VkExtent3D imageExtent;
		imageExtent.width = texWidth;
		imageExtent.height = texHeight;
		imageExtent.depth = 1;

		VkImageCreateInfo CreateImageInfo{};
		CreateImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		CreateImageInfo.pNext = nullptr;
		CreateImageInfo.imageType = VK_IMAGE_TYPE_2D;
		CreateImageInfo.format = image_format;
		CreateImageInfo.extent = imageExtent;
		CreateImageInfo.mipLevels = 1;
		CreateImageInfo.arrayLayers = 1;
		CreateImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		CreateImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		CreateImageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

		AllocatedImage newImage;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		vmaCreateImage(GraphicsContext::GetAllocator(), &CreateImageInfo, &allocInfo, &newImage.Image, &newImage.Allocation, nullptr);



		GraphicsContext::ImmediateSubmit([&](VkCommandBuffer cmd)
			{
				VkImageSubresourceRange range;
				range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				range.baseMipLevel = 0;
				range.levelCount = 1;
				range.baseArrayLayer = 0;
				range.layerCount = 1;

				VkImageMemoryBarrier imageBarrier_toTransfer = {};
				imageBarrier_toTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

				imageBarrier_toTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				imageBarrier_toTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				imageBarrier_toTransfer.image = newImage.Image;
				imageBarrier_toTransfer.subresourceRange = range;

				imageBarrier_toTransfer.srcAccessMask = 0;
				imageBarrier_toTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

				vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrier_toTransfer);

				VkBufferImageCopy copyRegion = {};
				copyRegion.bufferOffset = 0;
				copyRegion.bufferRowLength = 0;
				copyRegion.bufferImageHeight = 0;

				copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				copyRegion.imageSubresource.mipLevel = 0;
				copyRegion.imageSubresource.baseArrayLayer = 0;
				copyRegion.imageSubresource.layerCount = 1;
				copyRegion.imageExtent = imageExtent;

				//copy the buffer into the image
				vkCmdCopyBufferToImage(cmd, buffer->GetBuffer(), newImage.Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

				VkImageMemoryBarrier imageBarrier_toReadable = imageBarrier_toTransfer;

				imageBarrier_toReadable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				imageBarrier_toReadable.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

				imageBarrier_toReadable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				imageBarrier_toReadable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				//barrier the image into the shader readable layout
				vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrier_toReadable);
			});

		GlobalDeletionQueue.PushFunction([=]
			{
				vmaDestroyImage(GraphicsContext::GetAllocator(), newImage.Image, newImage.Allocation);
			});
		VkImageViewCreateInfo depthViewInfo{};
		depthViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		depthViewInfo.pNext = nullptr;

		depthViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		depthViewInfo.image = newImage.Image;
		depthViewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
		depthViewInfo.subresourceRange.baseMipLevel = 0;
		depthViewInfo.subresourceRange.levelCount = 1;
		depthViewInfo.subresourceRange.baseArrayLayer = 0;
		depthViewInfo.subresourceRange.layerCount = 1;
		depthViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		vkCreateImageView(GraphicsContext::GetDevice(), &depthViewInfo, nullptr, &m_View);

		buffer->Destroy();
		TN_CORE_INFO("Texture loaded successfully ({0})", path.string().c_str());
		m_Image = newImage;
	}

	Ref<Texture> Texture::Create(const std::filesystem::path& path)
	{
		Ref<Texture> returnValue;
		AssetImporter::ImportTexture(path, returnValue);
		return returnValue;
	}
}
