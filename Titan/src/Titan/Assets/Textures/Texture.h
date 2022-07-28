#pragma once
#include <filesystem>

#include "Titan/Core/Core.h"
#include "stb_image.h"
#include "Titan/Rendering/VulkanAPI/Buffers/Buffers.h"

namespace Titan
{
	class Texture
	{
	public:
		Texture(const std::filesystem::path& path);
		Texture(Texture& other) = default;
		VkImageView& GetView() { return m_View; }
		void* GetPixelData() const
		{ return m_Pixels; }
		static Ref<Texture> Create(const std::filesystem::path& path);
	private:
		stbi_uc* m_Pixels;
		VkImageView m_View;
		AllocatedImage m_Image;
	};
}
