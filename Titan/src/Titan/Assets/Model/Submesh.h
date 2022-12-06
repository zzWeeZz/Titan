#pragma once
#include <Titan/Rendering/Vertices.h>
#include <Titan/Rendering/Buffers/VertexPacket.h>
namespace Titan
{
	struct Meshlet
	{
		uint32_t vertices[64];
		uint32_t indices[378];
		uint32_t vertexCount;
		uint32_t indexCount;
	};
	class Submesh
	{

	public:
		Submesh(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
		
		inline VertexPackage& GetVertexPackage() { return m_VertexPackage; }
		inline std::vector<Vertex>& GetVertices() { return m_Vertices; }
		inline std::vector<uint32_t>& GetIndices() { return m_Indices; }
		inline std::vector<Meshlet>& GetMeshlets() { return m_Meshlets; }
		inline const TitanID& GetID() { return m_ID; }
	private:
		VertexPackage m_VertexPackage;
		std::vector<Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;
		std::vector<Meshlet> m_Meshlets;
		TitanID m_ID; // TODO: this should be some form of structure.
	};
}