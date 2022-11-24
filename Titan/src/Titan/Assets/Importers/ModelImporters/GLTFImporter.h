#pragma once
#include "Titan/Rendering/Buffers/VertexPacket.h"

namespace tinygltf
{
	class Node;
	class Model;
}

namespace Titan
{
	class GLTFImporter
	{
	public:
		static void Import(const std::filesystem::path& filepath, VertexPackage& outVertex);
	private:
		static void LoadNode(const tinygltf::Node& node, const tinygltf::Model& model, std::vector<Vertex>& outVerties, std::vector<uint32_t>& outIndices);
	};
}
