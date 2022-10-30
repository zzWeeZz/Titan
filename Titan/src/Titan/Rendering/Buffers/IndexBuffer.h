#pragma once
#include "Titan/Utils/TitanAllocator.h"

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
		AllocatedBuffer& GetAllocatedBuffer() { return m_GpuBuffer; }
		uint32_t GetIndexCount();
		static Ref<IndexBuffer> Create(const IndexBufferInfo& info);
		~IndexBuffer();
	private:
		size_t m_IndexCount;
		AllocatedBuffer m_GpuBuffer;
	};
}