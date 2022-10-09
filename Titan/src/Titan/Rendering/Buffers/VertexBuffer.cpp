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
		auto heapPropDefault = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		auto buffer = CD3DX12_RESOURCE_DESC::Buffer(info.sizeOfData * info.sizeOfVertex);
		
		TN_DX_CHECK(GraphicsContext::Device()->CreateCommittedResource
		(
			&heapPropDefault,
			D3D12_HEAP_FLAG_NONE,
			&buffer,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(m_VertexBuffer.GetAddressOf())
		));

        m_VertexBuffer->SetName(info.debugName.c_str());

        ID3D12Resource* vBufferUploadHeap;
		auto heapPropUpload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		TN_DX_CHECK(GraphicsContext::Device()->CreateCommittedResource
		(
			&heapPropUpload,
			D3D12_HEAP_FLAG_NONE,
			&buffer,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&vBufferUploadHeap)
		));

		vBufferUploadHeap->SetName(info.debugName.c_str());

		D3D12_SUBRESOURCE_DATA vertexData = {};
		vertexData.pData = reinterpret_cast<BYTE*>(info.vertexData); // pointer to our vertex array
		vertexData.RowPitch = info.sizeOfData * info.sizeOfVertex; // size of all our triangle vertex data
		vertexData.SlicePitch = info.sizeOfData * info.sizeOfVertex; // also the size of our triangle vertex data

		UpdateSubresources(GraphicsContext::CommandList().Get(), m_VertexBuffer.Get(), vBufferUploadHeap, 0, 0, 1, &vertexData);

		auto resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
			m_VertexBuffer.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		GraphicsContext::CommandList()->ResourceBarrier(
			1,
			&resourceBarrier);
		TN_DX_CHECK(GraphicsContext::CommandList()->Close());
		GraphicsContext::ExecuteCommandList();

		m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
		m_VertexBufferView.StrideInBytes = info.sizeOfVertex;
		m_VertexBufferView.SizeInBytes = info.sizeOfData * info.sizeOfVertex;


    }
    void VertexBuffer::Bind()
    {
		GraphicsContext::CommandList()->IASetVertexBuffers(0, 1, &m_VertexBufferView);
    }
    Ref<VertexBuffer> VertexBuffer::Create(const VertexBufferInfo& info)
    {
        return CreateRef<VertexBuffer>(info);
    }
}