#pragma once
#include <string>
#include <vector>

#include "Titan/Rendering/VulkanAPI/Buffers/Buffers.h"

namespace Titan
{
	class GLTFImporter
	{
	public:
		static void Import(const std::string& filepath, std::vector<Vertex>& outVertex);
	};
}
