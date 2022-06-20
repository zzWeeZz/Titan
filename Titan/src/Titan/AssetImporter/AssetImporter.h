#pragma once
#include <filesystem>
#include <iosfwd>
#include <vector>

#include "Titan/Rendering/VulkanAPI/Buffers/Buffers.h"

namespace Titan
{
	class AssetImporter
	{
	public:
		static void ImportModel(const std::filesystem::path& path, std::vector<Vertex>& outVertex);
	};
}
