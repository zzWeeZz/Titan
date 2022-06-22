#pragma once
#include <filesystem>
#include <string>
#include <vector>

#include "Titan/Rendering/VulkanAPI/Buffers/Buffers.h"
#include "Titan/Rendering/VulkanAPI/Buffers/VertexArray.h"

namespace Titan
{
	class GLTFImporter
	{
	public:
		static void Import(const std::filesystem::path& filepath, Ref<VertexArray>& outVertex);
	};
}
