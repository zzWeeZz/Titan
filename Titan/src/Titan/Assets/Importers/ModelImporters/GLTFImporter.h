#pragma once
#include <vector>
#include "Titan/Rendering/Buffers/VertexPacket.h"


struct Vertex;

namespace tinygltf
{
	class Node;
	class Model;
}

namespace Titan
{
	class Submesh;


	class GLTFImporter
	{
	public:
		static void Import(const std::filesystem::path& filepath, std::vector<Submesh>& outVertex);
	private:
		static void LoadNode(const tinygltf::Node& node, const tinygltf::Model& model, std::vector<Vertex>& outVerties, std::vector<uint32_t>& outIndices);
	};
}
