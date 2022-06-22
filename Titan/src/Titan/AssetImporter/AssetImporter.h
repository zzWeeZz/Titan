#pragma once
#include <filesystem>
#include <iosfwd>
#include <unordered_map>
#include <vector>

#include "Titan/Core/TitanMemory.h"
#include "Titan/Rendering/VulkanAPI/Buffers/Buffers.h"
#include "Titan/Rendering/VulkanAPI/Buffers/VertexArray.h"

namespace Titan
{
	class AssetImporter
	{
	public:
		static void ImportModel(const std::filesystem::path& path, Ref<VertexArray>& vertexArray);
		static void Shutdown();
	private:
		// Returns true if it exists in the registry
		static bool CheckMeshRegistry(const std::string& key, Ref<VertexArray>& outVertex);
		inline static std::unordered_map<std::string, Ref<VertexArray>*> s_MeshRegistry;
	};
}
