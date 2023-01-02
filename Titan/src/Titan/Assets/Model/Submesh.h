#pragma once
#include <Titan/Rendering/Vertices.h>
namespace Titan
{
	class StorageBuffer;
	class GenericBuffer;
	struct Meshlet
	{
		uint32_t vertexOffset;
		uint32_t triangleOffset;
		uint32_t vertexCount;
		uint32_t triangleCount;
		uint32_t meshId;
		uint32_t padd[3];
	};
	class Submesh
	{

	public:
		Submesh(std::vector<RawVertex>& vertices, std::vector<uint32_t>& indices, size_t pathHash);
		
		void CreateBuffers();

		inline std::vector<RawVertex>& GetRawVertices() { return m_RawVertices; }
		inline std::vector<BufferVertex>& GetVertices() { return m_BufferVertices; }
		inline std::vector<uint32_t>& GetIndices() { return m_Indices; }
		inline std::vector<Meshlet>& GetMeshlets() { return m_Meshlets; }
		inline std::vector<uint32_t>& GetMeshletVertices() { return m_MeshletVertices; }
		inline Ref<GenericBuffer> GetVertexBuffer() { return m_VertexBuffer; }
		inline Ref<GenericBuffer> GetMeshletBuffer() { return m_MeshletBuffer; }
		inline Ref<GenericBuffer> GetTriangleBuffer() { return m_TriangleBuffer; }
		inline Ref<GenericBuffer> GetMeshletVertexBuffer() { return m_MeshletVertexBuffer; }
		inline const size_t& GetHash() { return m_Hash; }
		inline const TitanID& GetID() { return m_ID; }
	private:
		std::vector<RawVertex> m_RawVertices;
		std::vector<BufferVertex> m_BufferVertices;
		std::vector<uint32_t> m_Indices;
		std::vector<Meshlet> m_Meshlets;
		std::vector<uint32_t> m_MeshletVertices;
		Ref<GenericBuffer> m_VertexBuffer;
		Ref<GenericBuffer> m_TriangleBuffer;
		Ref<GenericBuffer> m_MeshletBuffer;
		Ref<GenericBuffer> m_MeshletVertexBuffer;
		size_t m_Hash; // TODO: this should be some form of structure.
		TitanID m_ID;
	};
}