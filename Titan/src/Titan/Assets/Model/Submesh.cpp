#include "TNpch.h"
#include "Submesh.h"
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
}