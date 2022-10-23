#include "TNpch.h"
#include "Texture.h"
#include "stb_image.h"
#include "Titan/Rendering/GraphicsContext.h"
namespace Titan
{
	Texture::Texture()
	{

	}

	void Texture::Initialize(const std::filesystem::path& path)
	{
		int texWidth, texHeight, texChannels;
		m_Pixels = stbi_load(path.string().c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

		if (!m_Pixels)
		{
			TN_CORE_ERROR("Failed to load texture file: {0}", path.string().c_str());
			return;
		}
		SetPath(path);
		m_Width = texWidth;
		m_Height = texHeight;
		
		size_t bufferSize = m_Width * m_Height * 4;

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = bufferSize;
		bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
		AllocatedBuffer cpuBuffer;

		TitanAllocator::Allocate(cpuBuffer, &bufferInfo, &allocInfo);

		void* mappedData = nullptr;
		TitanAllocator::MapMemory(cpuBuffer, mappedData);
		memcpy_s(mappedData, bufferSize, m_Pixels, bufferSize);
		TitanAllocator::UnMapMemory(cpuBuffer);

		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = static_cast<uint32_t>(texWidth);
		imageInfo.extent.height = static_cast<uint32_t>(texHeight);
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.flags = 0; // Optional

		VmaAllocationCreateInfo imgAllocInfo{};
		imgAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		TitanAllocator::Allocate(m_GpuImage, &imageInfo, &imgAllocInfo);

		GraphicsContext::GetDevice().ImmediateSubmit([=](VkCommandBuffer cmd)
			{
				VkImageSubresourceRange range;
				range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				range.baseMipLevel = 0;
				range.levelCount = 1;
				range.baseArrayLayer = 0;
				range.layerCount = 1;

				VkImageMemoryBarrier imageBarrierToTransfer{};
				imageBarrierToTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				imageBarrierToTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				imageBarrierToTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				imageBarrierToTransfer.image = m_GpuImage.Image;
				imageBarrierToTransfer.subresourceRange = range;
				imageBarrierToTransfer.srcAccessMask = 0;
				imageBarrierToTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

				vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrierToTransfer);

				VkBufferImageCopy copyRegion{};
				copyRegion.bufferOffset = 0;
				copyRegion.bufferRowLength = 0;
				copyRegion.bufferImageHeight = 0;

				copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				copyRegion.imageSubresource.mipLevel = 0;
				copyRegion.imageSubresource.baseArrayLayer = 0;
				copyRegion.imageSubresource.layerCount = 1;
				copyRegion.imageExtent = imageInfo.extent;

				vkCmdCopyBufferToImage(cmd, cpuBuffer.buffer, m_GpuImage.Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);
				VkImageMemoryBarrier imageBarrierToFrontTransfer{};
				imageBarrierToFrontTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				imageBarrierToFrontTransfer.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				imageBarrierToFrontTransfer.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageBarrierToFrontTransfer.image = m_GpuImage.Image;
				imageBarrierToFrontTransfer.subresourceRange = range;
				imageBarrierToFrontTransfer.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				imageBarrierToFrontTransfer.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrierToFrontTransfer);
			});
		TitanAllocator::DeAllocate(cpuBuffer);

		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = m_GpuImage.Image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		TN_VK_CHECK(vkCreateImageView(GraphicsContext::GetDevice().GetHandle(), &viewInfo, nullptr, &m_ImageView));
		TitanAllocator::QueueDeletion([&]() {vkDestroyImageView(GraphicsContext::GetDevice().GetHandle(), m_ImageView, nullptr); });


		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;

		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.maxAnisotropy = 1.0f;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		TN_VK_CHECK(vkCreateSampler(GraphicsContext::GetDevice().GetHandle(), &samplerInfo, nullptr, &m_Sampler));
		TitanAllocator::QueueDeletion([&]() {vkDestroySampler(GraphicsContext::GetDevice().GetHandle(), m_Sampler, nullptr); });
	}

	Ref<Texture> Texture::Create()
	{
		return CreateRef<Texture>();
	}
}