#pragma once
#include <vulkan/vulkan.h>
#include "Titan/Core/TitanFormats.h"
#include <unordered_map>
namespace Titan
{
	class SamplerLibrary
	{
	public:
		static void Add(const std::string& key, const Filter& filter, const Address& address, const MipmapMode& mipmapMode);
		static VkSampler& Get(const std::string& key);
	private:
		inline static std::unordered_map<std::string, VkSampler> s_Samplers;
	};
}