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
		auto heapPropDefault = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		auto buffer = CD3DX12_RESOURCE_DESC::Buffer(info.sizeOfArray * sizeof(DWORD));

		const size_t sizeWithStride = info.sizeOfArray * sizeof(DWORD);
		GraphicsContext::Reset();
		TN_DX_CHECK(GraphicsContext::Device()->CreateCommittedResource
		(
			&heapPropDefault,
			D3D12_HEAP_FLAG_NONE,
			&buffer,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(m_IndexBuffer.GetAddressOf())
		));

		m_IndexBuffer->SetName(info.debugName.c_str());

		ID3D12Resource* iBufferUploadHeap;
		auto heapPropUpload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		TN_DX_CHECK(GraphicsContext::Device()->CreateCommittedResource
		(
			&heapPropUpload,
			D3D12_HEAP_FLAG_NONE,
			&buffer,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&iBufferUploadHeap)
		));

		iBufferUploadHeap->SetName(info.debugName.c_str());

		D3D12_SUBRESOURCE_DATA vertexData = {};
		vertexData.pData = reinterpret_cast<BYTE*>(info.indexData); // pointer to our vertex array
		vertexData.RowPitch = sizeWithStride; // size of all our triangle vertex data
		vertexData.SlicePitch = sizeWithStride; // also the size of our triangle vertex data

		UpdateSubresources(GraphicsContext::CommandList().Get(), m_IndexBuffer.Get(), iBufferUploadHeap, 0, 0, 1, &vertexData);

		auto resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
			m_IndexBuffer.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_INDEX_BUFFER);
		GraphicsContext::CommandList()->ResourceBarrier(
			1,
			&resourceBarrier);
		TN_DX_CHECK(GraphicsContext::CommandList()->Close());
		GraphicsContext::ExecuteCommandList();

		m_IndexBufferView.BufferLocation = m_IndexBuffer->GetGPUVirtualAddress();
		m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
		m_IndexBufferView.SizeInBytes = static_cast<UINT>(sizeWithStride);
	}
	void IndexBuffer::Bind()
	{
		GraphicsContext::CommandList()->IASetIndexBuffer(&m_IndexBufferView);
	}
	Ref<IndexBuffer> IndexBuffer::Create(const IndexBufferInfo& info)
	{
		return CreateRef<IndexBuffer>(info);
	}
}