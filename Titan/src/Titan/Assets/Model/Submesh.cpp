#include "TNpch.h"
#include "Submesh.h"

#include "Titan/Rendering/Buffers/StorageBuffer.h"
namespace Titan
{
	Submesh::Submesh(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
	{
		VertexBufferInfo vInfo{};
		vInfo.vertexData = vertices.data();
		vInfo.sizeOfArray = vertices.size();
		vInfo.sizeOfVertex = sizeof(Vertex);
		m_VertexPackage.vertexBuffer = VertexBuffer::Create(vInfo);
		m_Vertices = vertices;
		IndexBufferInfo iInfo{};
		iInfo.indexData = indices.data();
		iInfo.sizeOfArray = indices.size();
		m_VertexPackage.indexBuffer = IndexBuffer::Create(iInfo);
		m_Indices = indices;
		m_ID = CreateID();
	}
	void Submesh::CreateBuffers()
	{
		StorageBufferInfo info{};
		info.data = m_Vertices.data();
		info.size = m_Vertices.size();
		info.stride = sizeof(Vertex);
		m_VertexBuffer = StorageBuffer::Create(info);

		info.data = m_Indices.data();
		info.size = m_Indices.size();
		info.stride = sizeof(uint32_t);
		m_TriangleBuffer = StorageBuffer::Create(info);

		info.data = m_Meshlets.data();
		info.size = m_Meshlets.size();
		info.stride = sizeof(Meshlet);
		m_MeshletBuffer = StorageBuffer::Create(info);
	}
}