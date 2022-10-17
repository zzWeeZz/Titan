#include "TNpch.h"
#include "Texture.h"
namespace Titan
{
	Texture::Texture()
	{

	}

	void Texture::Initialize(const std::filesystem::path& path)
	{

	}

	Ref<Texture> Texture::Create()
	{
		return CreateRef<Texture>();
	}
}