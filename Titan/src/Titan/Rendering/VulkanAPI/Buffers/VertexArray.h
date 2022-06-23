#pragma once
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "Titan/Core/Core.h"
namespace Titan
{
	class VertexArray
	{
	public:
		VertexArray(std::vector<Vertex>& vertices, std::vector<uint16_t>& indices);
		inline Ref<VertexBuffer<Vertex>>& GetVertexBuffer() { return m_VertexBuffer; }
		inline Ref<IndexBuffer>& GetIndexBuffer() { return m_IndexBuffer; }
		inline std::vector<Vertex>& GetVertexArray() { return m_Vertices; }
		inline std::vector<uint16_t>& GetIndexArray() { return m_Indices; }
		static Ref<VertexArray> Create(std::vector<Vertex>& vertices, std::vector<uint16_t>& indices);
	private:
		std::vector<Vertex> m_Vertices;
		std::vector<uint16_t> m_Indices;
		Ref<VertexBuffer<Vertex>> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;
	};

	inline VertexArray::VertexArray(std::vector<Vertex>& vertices, std::vector<uint16_t>& indices) : m_Vertices(
		vertices)
	{
		m_Indices = indices;
		m_VertexBuffer = VertexBuffer<Vertex>::Create(vertices);
		m_IndexBuffer = IndexBuffer::Create(indices);
	}

	inline Ref<VertexArray> VertexArray::Create(std::vector<Vertex>& vertices, std::vector<uint16_t>& indices)
	{
		return CreateRef<VertexArray>(vertices, indices);
	}
}
