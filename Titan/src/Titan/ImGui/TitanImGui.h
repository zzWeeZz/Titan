#pragma once
#include <vulkan/vulkan.h>
namespace Titan
{
	class TitanImGui
	{
	public:
		static void Initialize();
		static void HandleDescriptorSet(VkDescriptorSet& desc);
		static void HandleImageBarrier(VkImageMemoryBarrier& memBarrier, VkImageMemoryBarrier& memReturnBarrier);
		static void Begin();
		static void End();
		static void FlushDescriptors(const uint32_t& currentFrame);
	private:
		inline static PerFrameInFlight<std::vector<VkDescriptorSet>> s_HandledDescriptors;
		inline static std::vector<VkImageMemoryBarrier> s_HandledImageBarriers;
		inline static std::vector<VkImageMemoryBarrier> s_HandledImageReturnBarriers;
	};
}