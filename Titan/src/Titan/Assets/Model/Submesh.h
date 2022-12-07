#pragma once
#include <Titan/Rendering/Vertices.h>
#include <Titan/Rendering/Buffers/VertexPacket.h>
namespace Titan
{
	class StorageBuffer;
	struct Meshlet
	{
		uint32_t vertexOffset;
		uint32_t triangleOffset;
		uint32_t vertexCount;
		uint32_t triangleCount;
	};
	class Submesh
	{

	public:
		Submesh(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
		
		void CreateBuffers();

		inline VertexPackage& GetVertexPackage() { return m_VertexPackage; }
		inline std::vector<Vertex>& GetVertices() { return m_Vertices; }
		inline std::vector<uint32_t>& GetIndices() { return m_Indices; }
		inline std::vector<Meshlet>& GetMeshlets() { return m_Meshlets; }
		inline std::vector<uint32_t>& GetMeshletVertices() { return m_MeshletVertices; }
		inline Ref<StorageBuffer> GetVertexBuffer() { return m_VertexBuffer; }
		inline Ref<StorageBuffer> GetMeshletBuffer() { return m_MeshletBuffer; }
		inline Ref<StorageBuffer> GetTriangleBuffer() { return m_TriangleBuffer; }
		inline Ref<StorageBuffer> GetMeshletVertexBuffer() { return m_MeshletVertexBuffer; }
		inline const TitanID& GetID() { return m_ID; }
	private:
		VertexPackage m_VertexPackage;
		std::vector<Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;
		std::vector<Meshlet> m_Meshlets;
		std::vector<uint32_t> m_MeshletVertices;
		Ref<StorageBuffer> m_VertexBuffer;
		Ref<StorageBuffer> m_TriangleBuffer;
		Ref<StorageBuffer> m_MeshletBuffer;
		Ref<StorageBuffer> m_MeshletVertexBuffer;
		TitanID m_ID; // TODO: this should be some form of structure.
	};
}