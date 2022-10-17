#pragma once
#include <d3d12.h>
#include "Titan/Assets/Resource.h"
namespace Titan
{
	class Texture : public Resource
	{
	public:
		Texture();
		// Inherited via Resource
		virtual void Initialize(const std::filesystem::path& path) override;
		static Ref<Texture> Create();
	private:
		WinRef<ID3D12Resource> m_Texture;
	};
}