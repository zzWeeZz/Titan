#include "TNpch.h"
#include "Submesh.h"

#include <functional>

#include <meshoptimizer/src/meshoptimizer.h>

#include "Titan/Utils/Utils.h"

#include "Titan/Rendering/Buffers/StorageBuffer.h"
#include "Titan/Rendering/Buffers/GenericBuffer.h"


namespace Titan
{
	Submesh::Submesh(std::vector<RawVertex>& vertices, std::vector<uint32_t>& indices, size_t pathHash)
	{
		m_RawVertices = vertices;
		m_Indices = indices;
		m_Hash = 0;
		std::hash<size_t> hasher;
		m_Hash += hasher(vertices.size());
		m_Hash += hasher(indices.size());
		m_ID = CreateID();
	}
	void Submesh::CreateBuffers()
	{
		m_BufferVertices.resize(m_RawVertices.size());
		for (size_t i = 0; i < m_RawVertices.size(); ++i)
		{
			QuantizeVertexBuffer(m_RawVertices[i], m_BufferVertices[i]);
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
	void Submesh::QuantizeVertexBuffer(RawVertex& rawVertex, BufferVertex& bufferVertex)
	{

		
			
		bufferVertex.Position = rawVertex.Position;

		glm::vec2 encodedNormal = EncodeOctahedronVectors(rawVertex.Normal);
		bufferVertex.Normal[0] = static_cast<glm::u8>(meshopt_quantizeUnorm(encodedNormal.x, 8));
		bufferVertex.Normal[1] = static_cast<glm::u8>(meshopt_quantizeUnorm(encodedNormal.y, 8));
		
		encodedNormal = EncodeOctahedronVectors(rawVertex.Tangent);
		bufferVertex.Tangent[0] = static_cast<glm::u8>(meshopt_quantizeUnorm(encodedNormal.x, 8));
		bufferVertex.Tangent[1] = static_cast<glm::u8>(meshopt_quantizeUnorm(encodedNormal.y, 8));
		bufferVertex.Color = glm::vec4(rawVertex.Color, 1.f);
		bufferVertex.TexCoords = rawVertex.TexCoords;
	}
}