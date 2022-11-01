#pragma once
#include <vulkan/vulkan.h>
namespace Titan
{
	class TitanImGui
	{
	public:
		static void Initialize();
		static void HandleDescriptorSet(VkDescriptorSet& desc);
		static void Begin();
		static void End();
		static void FlushDescriptors();
	private:
		inline static std::vector<VkDescriptorSet> s_HndledDescriptors;
	};
}