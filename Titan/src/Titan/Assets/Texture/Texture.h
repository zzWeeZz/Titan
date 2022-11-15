#pragma once
#include "Titan/Assets/Resource.h"
#include "Titan/Utils/TitanAllocator.h"
namespace Titan
{
	class Texture : public Resource
	{
	public:
		Texture();
		// Inherited via Resource
		virtual void Initialize(const std::filesystem::path& path) override;
		VkDescriptorSet& GetDescriptorSet();
		AllocatedImage& GetAllocation() { return m_GpuImage; }
		VkSampler& GetSampler() { return m_Sampler; }
		VkImageView& GetView() { return m_ImageView; }
		static Ref<Texture> Create();
	private:
		void* m_Pixels;
		size_t m_Width;
		size_t m_Height;
		AllocatedImage m_GpuImage;
		VkImageView m_ImageView;
		VkSampler m_Sampler;
	};
}