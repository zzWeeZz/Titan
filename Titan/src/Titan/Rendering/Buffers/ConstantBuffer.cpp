#include "TNpch.h"
#include "ConstantBuffer.h"
#include "dx12helpers/d3dx12.h"
namespace Titan
{
	ConstantBuffer::ConstantBuffer()
	{
		
		for (size_t i = 0; i < g_FrameCount; ++i)
		{
			D3D12_DESCRIPTOR_HEAP_DESC desc{};
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			desc.NumDescriptors = 1;
			TN_DX_CHECK(GraphicsContext::Device()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_MainDescriptorHeap[i].ReleaseAndGetAddressOf())));

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

			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
			cbvDesc.BufferLocation = m_CBufferUploadHeap[i]->GetGPUVirtualAddress();
			cbvDesc.SizeInBytes = (sizeof(ConstantBuffer) + 255) & ~255;    // CB size is required to be 256-byte aligned.
			GraphicsContext::Device()->CreateConstantBufferView(&cbvDesc, m_MainDescriptorHeap[i]->GetCPUDescriptorHandleForHeapStart());
		}
	}
	void ConstantBuffer::SetData(void* data, size_t sizeOfData)
	{
		auto currCBUploadHeap = m_CBufferUploadHeap[GraphicsContext::GetFrameIndex()];
		CD3DX12_RANGE readRange(0, 0);    // We do not intend to read from this resource on the CPU. (End is less than or equal to begin)
		currCBUploadHeap->Map(0, &readRange, reinterpret_cast<void**>(&m_ConstantBufferGPUAddress[GraphicsContext::GetFrameIndex()]));
		memcpy(m_ConstantBufferGPUAddress[GraphicsContext::GetFrameIndex()], data, sizeOfData);
		currCBUploadHeap->Unmap(0, &readRange);
	}
	void ConstantBuffer::Bind(const uint32_t bindPoint)
	{
		ID3D12DescriptorHeap* descriptorHeaps[] = { m_MainDescriptorHeap[GraphicsContext::GetFrameIndex()].Get() };
		GraphicsContext::CommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

		// set the root descriptor table 0 to the constant buffer descriptor heap
		GraphicsContext::CommandList()->SetGraphicsRootDescriptorTable(0, m_MainDescriptorHeap[GraphicsContext::GetFrameIndex()]->GetGPUDescriptorHandleForHeapStart());
	}
	Ref<ConstantBuffer> ConstantBuffer::Create()
	{
		return CreateRef<ConstantBuffer>();
	}
}