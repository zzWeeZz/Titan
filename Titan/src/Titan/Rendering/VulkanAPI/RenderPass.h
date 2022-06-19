#pragma once
#include "Titan/Core/Core.h"
#include <vulkan/vulkan.h>
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
		explicit RenderPass(const RenderPassCreateInfo& info);
		void Shutdown();
		static Ref<RenderPass> Create(const RenderPassCreateInfo& info);
	private:
		VkRenderPass m_RenderPass;
	};
}
