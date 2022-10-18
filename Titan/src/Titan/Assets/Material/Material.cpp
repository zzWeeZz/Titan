#include "TNpch.h"
#include "Material.h"
namespace Titan
{
	Material::Material()
	{

	}

	void Material::Initialize(const std::filesystem::path& path)
	{
		SetPath(path);
		if (path.empty())
		{
			return;
		}
		TN_CORE_ERROR("MATERIAL READING NOT IMPLEMENTED");
	}

	void Material::SetTexture(TitanID textureId)
	{
		m_TextureID = textureId;
	}

	inline Ref<Material> Titan::Material::Create()
	{
		return CreateRef<Material>();
	}
}