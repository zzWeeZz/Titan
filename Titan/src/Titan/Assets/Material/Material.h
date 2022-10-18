#pragma once
#include "Titan/Assets/Resource.h"
namespace Titan
{
	class Material : public Resource
	{
	public:
		Material();

		// Inherited via Resource
		virtual void Initialize(const std::filesystem::path& path) override;

		void SetTexture(TitanID textureId);

		inline TitanID GetTextureID() const { return m_TextureID; }

		static Ref<Material> Create();

	private:
		TitanID m_TextureID;
	};
}
