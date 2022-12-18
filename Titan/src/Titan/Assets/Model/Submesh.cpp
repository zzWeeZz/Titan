#include "TNpch.h"
#include "Submesh.h"

#include "Titan/Rendering/Buffers/StorageBuffer.h"
#include "Titan/Rendering/Buffers/GenericBuffer.h"
namespace Titan
{
	Submesh::Submesh(std::vector<RawVertex>& vertices, std::vector<uint32_t>& indices)
	{
		m_RawVertices = vertices;
		m_Indices = indices;
		m_Hash = 0;
		m_Hash += vertices.size();
		m_Hash += indices.size();
		m_ID = CreateID();
	}
	void Submesh::CreateBuffers()
	{
		m_BufferVertices.resize(m_RawVertices.size());
		for (size_t i = 0; i < m_RawVertices.size(); ++i)
		{
			auto& bufferVert = m_BufferVertices[i];
			auto& rawVert = m_RawVertices[i];

			bufferVert.Position = glm::vec4(rawVert.Position, 1.f);
			bufferVert.Normal = glm::vec4(rawVert.Normal, 1.f);
			bufferVert.Tangent = glm::vec4(rawVert.Tangent, 1.f);
			bufferVert.Color = glm::vec4(rawVert.Color, 1.f);
			bufferVert.TexCoords = rawVert.TexCoords;
		}

		GenericBufferInfo vertexBufferInfo{};
		vertexBufferInfo.data = m_BufferVertices.data();
		vertexBufferInfo.size = m_BufferVertices.size();
		vertexBufferInfo.stride = sizeof(BufferVertex);
		vertexBufferInfo.perFrameInFlight = false;
		vertexBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		vertexBufferInfo.allocUsage = VMA_MEMORY_USAGE_CPU_ONLY;

		m_VertexBuffer = GenericBuffer::Create(vertexBufferInfo);


		vertexBufferInfo.data = m_Indices.data();
		vertexBufferInfo.size = m_Indices.size();
		vertexBufferInfo.stride = sizeof(uint32_t);
		m_TriangleBuffer = GenericBuffer::Create(vertexBufferInfo);
		

		vertexBufferInfo.data = m_MeshletVertices.data();
		vertexBufferInfo.size = m_MeshletVertices.size();

		m_MeshletVertexBuffer = GenericBuffer::Create(vertexBufferInfo);


		GenericBufferInfo meshletBufferInfo{};
		meshletBufferInfo.data = m_Meshlets.data();
		meshletBufferInfo.size = m_Meshlets.size();
		meshletBufferInfo.stride = sizeof(Meshlet);
		meshletBufferInfo.perFrameInFlight = false;
		meshletBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		meshletBufferInfo.allocUsage = VMA_MEMORY_USAGE_CPU_ONLY;

		m_MeshletBuffer = GenericBuffer::Create(meshletBufferInfo);

		//info.data = m_Meshlets.data();
		//info.size = m_Meshlets.size();
		//info.stride = sizeof(Meshlet);
		//m_MeshletBuffer = StorageBuffer::Create(info);

		m_Hash += m_Meshlets.size();
	}
}