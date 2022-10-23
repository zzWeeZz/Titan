#include "TNpch.h"
#include "IndexBuffer.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <dx12helpers/d3dx12.h>

#include "Titan/Rendering/GraphicsContext.h"
namespace Titan
{
	IndexBuffer::IndexBuffer(const IndexBufferInfo& info)
	{
		
	}
	void IndexBuffer::Bind()
	{
	}
	size_t IndexBuffer::GetIndexCount()
	{
		return	m_IndexCount;
	}
	Ref<IndexBuffer> IndexBuffer::Create(const IndexBufferInfo& info)
	{
		return CreateRef<IndexBuffer>(info);
	}
	IndexBuffer::~IndexBuffer()
	{
		
	}
}