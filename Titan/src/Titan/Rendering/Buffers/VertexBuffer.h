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
		std::string debugName = "";
	};
	class VertexBuffer
	{
	public:
		VertexBuffer(const VertexBufferInfo& info);

		void Bind();

		static Ref<VertexBuffer> Create(const VertexBufferInfo& info);
	private:
		size_t m_SizeOfVertex;

	};
}