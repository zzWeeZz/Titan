#pragma once
#include <Titan/Rendering/Vertices.h>
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
		Submesh(std::vector<RawVertex>& vertices, std::vector<uint32_t>& indices);
		

		
		void CreateBuffers();

		inline std::vector<RawVertex>& GetRawVertices() { return m_RawVertices; }
		inline std::vector<BufferVertex>& GetVertices() { return m_BufferVertices; }
		inline std::vector<uint32_t>& GetIndices() { return m_Indices; }
		inline std::vector<Meshlet>& GetMeshlets() { return m_Meshlets; }
		inline std::vector<uint32_t>& GetMeshletVertices() { return m_MeshletVertices; }
		inline Ref<StorageBuffer> GetVertexBuffer() { return m_VertexBuffer; }
		inline Ref<StorageBuffer> GetMeshletBuffer() { return m_MeshletBuffer; }
		inline Ref<StorageBuffer> GetTriangleBuffer() { return m_TriangleBuffer; }
		inline Ref<StorageBuffer> GetMeshletVertexBuffer() { return m_MeshletVertexBuffer; }
		inline const size_t& GetHash() { return m_ID; }
	private:
		std::vector<RawVertex> m_RawVertices;
		std::vector<BufferVertex> m_BufferVertices;
		std::vector<uint32_t> m_Indices;
		std::vector<Meshlet> m_Meshlets;
		std::vector<uint32_t> m_MeshletVertices;
		Ref<StorageBuffer> m_VertexBuffer;
		Ref<StorageBuffer> m_TriangleBuffer;
		Ref<StorageBuffer> m_MeshletBuffer;
		Ref<StorageBuffer> m_MeshletVertexBuffer;
		size_t m_ID; // TODO: this should be some form of structure.
	};
}