#include "TNpch.h"
#include "SamplerLibrary.h"
#include "Titan/Utils/TitanAllocator.h"
#include "Titan/Rendering/GraphicsContext.h"

void Titan::SamplerLibrary::Add(const std::string& key, const Filter& filter, const Address& address, const MipmapMode& mipmapMode)
{
    VkSamplerCreateInfo info{};
    info.magFilter = FormatToVkFormat(filter);
    info.minFilter = FormatToVkFormat(filter);
    info.addressModeU = FormatToVkFormat(address);
    info.addressModeV = FormatToVkFormat(address);
    info.addressModeW = FormatToVkFormat(address);
    info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    info.unnormalizedCoordinates = VK_FALSE;
    info.compareEnable = VK_FALSE;
    info.compareOp = VK_COMPARE_OP_ALWAYS;
	info.mipmapMode = FormatToVkFormat(mipmapMode);
	info.mipLodBias = 0.0f;
	info.minLod = 0.0f;
	info.maxLod = 0.0f;
	info.anisotropyEnable = VK_FALSE;
	info.maxAnisotropy = 1.0f;
    s_Samplers[key] = VkSampler();
    TN_VK_CHECK(vkCreateSampler(GraphicsContext::GetDevice().GetHandle(), &info, nullptr, &s_Samplers[key]));
    TitanAllocator::QueueDeletion([&, key]() {vkDestroySampler(GraphicsContext::GetDevice().GetHandle(), s_Samplers[key], nullptr); });
}

VkSampler& Titan::SamplerLibrary::Get(const std::string& key)
{
#ifdef TN_CONFIG_DEBUG
    if (s_Samplers.contains(key))
    {
        TN_CORE_ERROR("Samper {0} does not exist!", key);
    }
#endif // TN_CONFIG_DEBUG

    return s_Samplers[key];
}
