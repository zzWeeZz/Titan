#include "TNpch.h"
#include "ConstantBuffer.h"
#include "dx12helpers/d3dx12.h"
namespace Titan
{
	ConstantBuffer::ConstantBuffer(size_t sizeOfObject)
	{
		for (size_t i = 0; i < g_FrameCount; ++i)
		{
			m_ConstantBufferGPUAddress[i] = nullptr;
			auto upload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			auto resource = CD3DX12_RESOURCE_DESC::Buffer(1024 * 64);
			GraphicsContext::Device()->CreateCommittedResource(
				&upload,
				D3D12_HEAP_FLAG_NONE,
				&resource,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(m_CBufferUploadHeap[i].GetAddressOf()));
			m_CBufferUploadHeap[i]->SetName(L"Constant Buffer Upload Resource Heap");
		}
	}
	void ConstantBuffer::SetData(void* data, size_t sizeOfData, size_t drawID)
	{
		auto currCBUploadHeap = m_CBufferUploadHeap[GraphicsContext::GetFrameIndex()];
		CD3DX12_RANGE readRange(0, 0);    // We do not intend to read from this resource on the CPU. (End is less than or equal to begin)
		size_t cBufferOffset = (sizeOfData + 255) & ~255;
		cBufferOffset *= drawID;
		currCBUploadHeap->Map(0, &readRange, reinterpret_cast<void**>(&m_ConstantBufferGPUAddress[GraphicsContext::GetFrameIndex()]));
		memcpy(m_ConstantBufferGPUAddress[GraphicsContext::GetFrameIndex()] + cBufferOffset, data, sizeOfData);
		currCBUploadHeap->Unmap(0, &readRange);
	}
	void ConstantBuffer::Bind(uint32_t drawID, size_t sizeOfData)
	{
		size_t cBufferOffset = (sizeOfData + 255) & ~255;
		cBufferOffset *= drawID;
		GraphicsContext::CommandList()->SetGraphicsRootConstantBufferView(0, m_CBufferUploadHeap[GraphicsContext::GetFrameIndex()]->GetGPUVirtualAddress() + cBufferOffset);
		}
	Ref<ConstantBuffer> ConstantBuffer::Create(size_t sizeOfObject)
	{
		return CreateRef<ConstantBuffer>(sizeOfObject);
	}
	ConstantBuffer::~ConstantBuffer()
	{
		for (size_t i = 0; i < g_FrameCount; ++i)
		{
			TN_SAFE_RELEASE(m_CBufferUploadHeap[i]);
		}
	}
}