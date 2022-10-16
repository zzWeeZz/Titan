#pragma once
#include "Titan/Assets/Resource.h"
#include <Titan/Rendering/Buffers/VertexPacket.h>
namespace Titan
{
	class ModelHandle : public Resource
	{
	public:
		ModelHandle() = default;
		void Initialize(const std::filesystem::path& path) override;
		VertexPackage& GetVertexPackage() { return m_VertexPackage; }
		static Ref<ModelHandle> Create() { return CreateRef<ModelHandle>(); }
	private:
		VertexPackage m_VertexPackage;
	};
}
