#include "TNpch.h"
#include "ConstantBuffer.h"
#include "dx12helpers/d3dx12.h"
namespace Titan
{
	ConstantBuffer::ConstantBuffer(size_t sizeOfObject)
	{

	}
	void ConstantBuffer::SetData(void* data, size_t sizeOfData, size_t drawID)
	{

	}
	void ConstantBuffer::Bind(uint32_t drawID, size_t sizeOfData)
	{

	}
	Ref<ConstantBuffer> ConstantBuffer::Create(size_t sizeOfObject)
	{
		return CreateRef<ConstantBuffer>(sizeOfObject);
	}
	ConstantBuffer::~ConstantBuffer()
	{
		
	}
}