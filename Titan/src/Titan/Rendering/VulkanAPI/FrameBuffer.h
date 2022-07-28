#pragma once
#include "Titan/Core/Core.h"
#include <vulkan/vulkan.h>

#include "Buffers/Buffers.h"

namespace Titan
{
	struct FrameBufferInfo
	{
		bool shouldResize;
		uint32_t Width;
		uint32_t Height;
		VkRenderPass RenderPass;
		std::vector<VkImageView> Views;
		VkImageView DepthViews;
		bool isSwapchain = false;
	};

	class FrameBuffer
	{
	public:
		explicit FrameBuffer(const FrameBufferInfo& spec);
		inline VkFramebuffer& GetFrameBuffer(uint32_t imageIndex) { return m_FrameBuffers[imageIndex]; }
		inline VkImage& GetImage(uint32_t imageIndex) { return m_AllocatedImages[imageIndex].Image; }
		inline VkImageView& GetImageView(uint32_t imageIndex) { return m_Views[imageIndex]; }
		inline std::vector<AllocatedImage>& GetImages() { return m_AllocatedImages; }
		inline std::vector<VkImageView>& GetViews() { return m_Views; }
		void Shutdown();
		static Ref<FrameBuffer> Create(const FrameBufferInfo& spec);
	private:
		std::vector<VkFramebuffer> m_FrameBuffers;
		std::vector<VkImageView> m_Views;
		std::vector<AllocatedImage> m_AllocatedImages;
	};
}
