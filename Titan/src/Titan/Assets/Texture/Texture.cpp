#include "TNpch.h"
#include "Texture.h"
#include "stb_image.h"

namespace Titan
{
	Texture::Texture()
	{

	}

	void Texture::Initialize(const std::filesystem::path& path)
	{
		int texWidth, texHeight, texChannels;
		m_Pixels = stbi_load(path.string().c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

		if (!m_Pixels)
		{
			TN_CORE_ERROR("Failed to load texture file: {0}", path.string().c_str());
			return;
		}
		SetPath(path);
		m_Width = texWidth;
		m_Height = texHeight;

	}

	Ref<Texture> Texture::Create()
	{
		return CreateRef<Texture>();
	}
}