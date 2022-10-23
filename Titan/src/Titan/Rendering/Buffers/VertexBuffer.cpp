#include "TNpch.h"
#include "VertexBuffer.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <dx12helpers/d3dx12.h>
#include "Titan/Rendering/GraphicsContext.h"
namespace Titan
{
    VertexBuffer::VertexBuffer(const VertexBufferInfo& info)
    {
		

    }
    void VertexBuffer::Bind()
    {
    }
    Ref<VertexBuffer> VertexBuffer::Create(const VertexBufferInfo& info)
    {
        return CreateRef<VertexBuffer>(info);
    }
}