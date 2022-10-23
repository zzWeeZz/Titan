#pragma once
#include "Titan/Core/TitanMemory.h"
#include "Titan/Utils/TitanAllocator.h"
#include <d3d12.h>
#include <string>
namespace Titan
{
	struct VertexBufferInfo
	{
		void* vertexData = nullptr;
		size_t sizeOfArray = 0;
		size_t sizeOfVertex = 0;
		std::string debugName = "";
	};
	class VertexBuffer
	{
	public:
		VertexBuffer(const VertexBufferInfo& info);

		AllocatedBuffer& GetAllocation() { return m_GpuBuffer; }

		static Ref<VertexBuffer> Create(const VertexBufferInfo& info);
	private:
		AllocatedBuffer m_GpuBuffer;
	};
}