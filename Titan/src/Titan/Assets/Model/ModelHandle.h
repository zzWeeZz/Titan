#pragma once
#include "Titan/Assets/Resource.h"


#include <Titan/Rendering/Buffers/VertexPacket.h>

namespace Titan
{
	class Submesh;

	class ModelHandle : public Resource
	{
	public:
		ModelHandle() = default;
		void Initialize(const std::filesystem::path& path) override;
		std::vector<Submesh>& GetSubMeshes() { return m_Submeshes; }
		static Ref<ModelHandle> Create() { return CreateRef<ModelHandle>(); }
	private:
		std::vector<Submesh> m_Submeshes;
	};
}
