#pragma once
#include "Titan/Core/TitanMemory.h"
#include <d3d12.h>
#include <string>
namespace Titan
{
	struct VertexBufferInfo
	{
		void* vertexData = nullptr;
		size_t sizeOfData = 0;
		size_t sizeOfVertex = 0;
		std::wstring debugName = L"";
	};
	class VertexBuffer
	{
	public:
		VertexBuffer(const VertexBufferInfo& info);

		void Bind();

		static Ref<VertexBuffer> Create(const VertexBufferInfo& info);
	private:
		size_t m_SizeOfVertex;

		WinRef<ID3D12Resource> m_VertexBuffer; // a default buffer in GPU memory that we will load vertex data for our triangle into

		D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView; // a structure containing a pointer to the vertex data in gpu memory
												   // the total size of the buffer, and the size of each element (vertex)
	};
}