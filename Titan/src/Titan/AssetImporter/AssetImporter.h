#pragma once
#include <filesystem>
#include <iosfwd>
#include <unordered_map>
#include <vector>

#include "Titan/Assets/Textures/Texture.h"
#include "Titan/Core/TitanMemory.h"
#include "Titan/Rendering/VulkanAPI/Buffers/Buffers.h"
#include "Titan/Rendering/VulkanAPI/Buffers/VertexArray.h"

namespace Titan
{
	class AssetImporter
	{
	public:
		static void ImportModel(const std::filesystem::path& path, Ref<VertexArray>& vertexArray);
		static void ImportTexture(const std::filesystem::path& path, Ref<Texture>& texture);
		static void Shutdown();
	private:
		// Returns true if it exists in the registry
		static bool CheckMeshRegistry(const std::string& key, Ref<VertexArray>& outVertex);
		static bool CheckTextureRegistry(const std::string& key, Ref<Texture>& outTexture);
		inline static std::unordered_map<std::string, Ref<VertexArray>*> s_MeshRegistry;
		inline static std::unordered_map<std::string, Ref<Texture>*> s_TextureRegistry;
	};
}
