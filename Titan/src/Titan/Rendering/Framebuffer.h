#pragma once
#include <vulkan/vulkan.h>
#include <Titan/Core/TitanFormats.h>
#include "Titan/Core/Core.h"
#include <Titan/Utils/TitanAllocator.h>
#include "Titan/Rendering/GraphicsContext.h"
namespace Titan
{
	struct FramebufferInfo
	{
		size_t width;
		size_t height;
		std::vector<ImageFormat> imageFormats;
	};

	class Framebuffer
	{
	public:
		Framebuffer(const FramebufferInfo& info);

		void Resize(size_t width, size_t height);

		TN_FORCE_INLINE std::vector<AllocatedImage> GetImages() { return m_Images[GraphicsContext::GetCurrentFrame()]; }
		TN_FORCE_INLINE std::vector<VkImageView> GetViews() { return m_Views[GraphicsContext::GetCurrentFrame()]; }
		TN_FORCE_INLINE AllocatedImage& GetAllocatedImage(size_t index) { return m_Images[GraphicsContext::GetCurrentFrame()][index]; }
		TN_FORCE_INLINE VkImageView& GetImageView(size_t index) { return m_Views[GraphicsContext::GetCurrentFrame()][index]; }

		TN_FORCE_INLINE VkRect2D GetRect() const { return m_Rect; }

		VkDescriptorSet GetDescriptorSet();
		FramebufferInfo& GetInfo() { return m_Info; }
		void Bind(VkCommandBuffer& cmd);
		void CleanUp();
		static Ref<Framebuffer> Create(const FramebufferInfo& info);
	private:
		void Validate();
		FramebufferInfo m_Info;
		VkRect2D m_Rect;
		PerFrameInFlight<std::vector<AllocatedImage>> m_Images;
		PerFrameInFlight<std::vector<VkImageView>> m_Views;
	};
}

