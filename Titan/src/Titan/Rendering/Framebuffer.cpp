#include "TNpch.h"
#include "Framebuffer.h"
#include "Titan/Rendering/GraphicsContext.h"
#include <backends/imgui_impl_vulkan.h>
#include "Titan/Rendering/Libraries/SamplerLibrary.h"
#include "Titan/ImGui/TitanImGui.h"
namespace Titan
{
    Framebuffer::Framebuffer(const FramebufferInfo& info) 
    {
        m_Info = info;
        for (size_t i = 0; i < g_FramesInFlight; i++)
        {
            m_Images[i].resize(m_Info.imageFormats.size());
            m_Views[i].resize(m_Info.imageFormats.size());
        }
        Validate();
    }

    void Framebuffer::Resize(size_t width, size_t height)
    {
		if (width == 0 || height == 0)
		{
			TN_CORE_ASSERT(false, "resize failed, width or height were 0!");
		}
		m_Info.height = height;
		m_Info.width = width;
		CleanUp();
		Validate();
    }

	void Framebuffer::Bind(VkCommandBuffer& cmd)
	{
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(GetInfo().width);
		viewport.height = static_cast<float>(GetInfo().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(cmd, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent.width = static_cast<uint32_t>(GetInfo().width);
		scissor.extent.height = static_cast<uint32_t>(GetInfo().height);
		vkCmdSetScissor(cmd, 0, 1, &scissor);
	}

	VkDescriptorSet Framebuffer::GetDescriptorSet()
	{
		auto desc = ImGui_ImplVulkan_AddTexture(SamplerLibrary::Get("Clamp"), GetViews()[0], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		TitanImGui::HandleDescriptorSet(desc);
		VkImageMemoryBarrier image_memory_barrier
		{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			.image = GetImages()[0].Image,
			.subresourceRange =
			{
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1,
			}
		};
		VkImageMemoryBarrier image_Returnmemory_barrier
		{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.image = GetImages()[0].Image,
			.subresourceRange =
			{
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1,
			}
		};
		TitanImGui::HandleImageBarrier(image_memory_barrier, image_Returnmemory_barrier);
		return desc;
	}

    void Framebuffer::CleanUp()
    {
		GraphicsContext::GetDevice().WaitForIdle();
		for (size_t i = 0; i < g_FramesInFlight; ++i)
		{
			for (size_t imageFormatIndex = 0; imageFormatIndex < m_Info.imageFormats.size(); ++imageFormatIndex)
			{
				vkDestroyImageView(GraphicsContext::GetDevice().GetHandle(), m_Views[i][imageFormatIndex], nullptr);
			}
		}
		for (size_t i = 0; i < g_FramesInFlight; ++i)
		{
			for (size_t imageFormatIndex = 0; imageFormatIndex < m_Info.imageFormats.size(); ++imageFormatIndex)
			{
				TitanAllocator::DeAllocate(m_Images[i][imageFormatIndex]);
			}
		}
    }

    Ref<Framebuffer> Framebuffer::Create(const FramebufferInfo& info)
    {
        return CreateRef<Framebuffer>(info);
    }

    void Framebuffer::Validate()
    {
        for (size_t i = 0; i < g_FramesInFlight; ++i)
        {
            for (size_t imageFormatIndex = 0; imageFormatIndex < m_Info.imageFormats.size(); ++imageFormatIndex)
            {
				if (IsDepth(m_Info.imageFormats[imageFormatIndex]))
				{
					TN_CORE_WARN("Depth views are not supported yet!");
					continue;
				}
				VkImageCreateInfo imageInfo{};
				imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
				imageInfo.imageType = VK_IMAGE_TYPE_2D;
				imageInfo.extent.width = static_cast<uint32_t>(m_Info.width);
				imageInfo.extent.height = static_cast<uint32_t>(m_Info.height);
				imageInfo.extent.depth = 1;
				imageInfo.mipLevels = 1;
				imageInfo.arrayLayers = 1;
				imageInfo.format = FormatToVkFormat(m_Info.imageFormats[imageFormatIndex]);
				imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
				imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
				imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
				imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
				imageInfo.flags = 0; // Optional
		
				VmaAllocationCreateInfo imgAllocInfo{};
				imgAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

				TitanAllocator::Allocate(m_Images[i][imageFormatIndex], &imageInfo, &imgAllocInfo);

				VkImageViewCreateInfo viewInfo{};
				viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				viewInfo.image = m_Images[i][imageFormatIndex].Image;
				viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
				viewInfo.format = FormatToVkFormat(m_Info.imageFormats[imageFormatIndex]);
				viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT /*| VK_IMAGE_USAGE_SAMPLED_BIT*/;
				viewInfo.subresourceRange.baseMipLevel = 0;
				viewInfo.subresourceRange.levelCount = 1;
				viewInfo.subresourceRange.baseArrayLayer = 0;
				viewInfo.subresourceRange.layerCount = 1;

				TN_VK_CHECK(vkCreateImageView(GraphicsContext::GetDevice().GetHandle(), &viewInfo, nullptr, &m_Views[i][imageFormatIndex]));
            }
        }
    }
}