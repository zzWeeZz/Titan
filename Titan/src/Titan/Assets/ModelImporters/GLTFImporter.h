#pragma once
#include "Titan/Rendering/Buffers/VertexPacket.h"

namespace Titan
{
	class GLTFImporter
	{
	public:
		static void Import(const std::filesystem::path& filepath, VertexPackage& outVertex);
	};
}
