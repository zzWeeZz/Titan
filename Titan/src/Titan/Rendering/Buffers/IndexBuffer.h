#pragma once
#include <d3d12.h>
#include "Titan/Core/TitanMemory.h"
namespace Titan
{
	struct IndexBufferInfo
	{
		void* indexData;
		size_t sizeOfArray;
		std::wstring debugName;
	};

	class IndexBuffer
	{
	public:
		IndexBuffer(const IndexBufferInfo& info);
		void Bind();
		size_t GetIndexCount();
		static Ref<IndexBuffer> Create(const IndexBufferInfo& info);
		~IndexBuffer();
	private:
		size_t m_IndexCount;
	};
}