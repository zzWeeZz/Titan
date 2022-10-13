#include "TNpch.h"
#include "Framebuffer.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <dx12helpers/d3dx12.h>
namespace Titan
{
	
	Framebuffer::Framebuffer(const FramebufferInfo& info) : m_RtvDescriptorSize(0)
	{
		m_FBInfo = info;
		for (size_t i = 0; i < g_FrameCount; ++i)
		{
			m_RenderTargets[i].resize(m_FBInfo.imageFormats.size());
		}
		Validate();
	}
	void Framebuffer::Resize(const size_t width, const size_t height)
	{
	}
	void Framebuffer::Clear(const glm::vec4& color)
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), GraphicsContext::GetFrameIndex(), m_RtvDescriptorSize);
		const float clearColor[] = { color.x, color.y, color.z, color.a };
		GraphicsContext::CommandList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		GraphicsContext::CommandList()->ClearDepthStencilView(m_DsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr);
	}
	WinRef<ID3D12Resource> Framebuffer::GetCurrentRtv()
	{
		return m_RenderTargets[GraphicsContext::GetFrameIndex()][0];
	}
	void Framebuffer::Bind()
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), GraphicsContext::GetFrameIndex(), m_RtvDescriptorSize);
		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_DsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
		GraphicsContext::CommandList()->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
	}
	void Framebuffer::Unbind()
	{
	}
	Ref<Framebuffer> Titan::Framebuffer::Create(const FramebufferInfo& info)
	{
		return CreateRef<Framebuffer>(info);
	}
	void Framebuffer::Validate()
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = g_FrameCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		TN_DX_CHECK(GraphicsContext::Device()->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(m_RtvDescriptorHeap.GetAddressOf())));

		m_RtvDescriptorSize = GraphicsContext::Device()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

		auto device = GraphicsContext::Device();
		for (size_t i = 0; i < g_FrameCount; ++i)
		{
			for (size_t j = 0; j < m_FBInfo.imageFormats.size(); ++j)
			{
				if (!IsDepth(m_FBInfo.imageFormats[j]))
				{
					D3D12_RESOURCE_DESC texDesc = {};
					texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
					texDesc.Width = m_FBInfo.width;
					texDesc.Height = (UINT)m_FBInfo.height;
					texDesc.DepthOrArraySize = 1;
					texDesc.MipLevels = 1;
					texDesc.Format = FormatToDXFormat(m_FBInfo.imageFormats[j]);
					texDesc.SampleDesc.Count = 1;
					texDesc.SampleDesc.Quality = 0;
					texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
					texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

					D3D12_HEAP_PROPERTIES HeapProps;
					HeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
					HeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
					HeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
					HeapProps.CreationNodeMask = 1;
					HeapProps.VisibleNodeMask = 1;
					
					FLOAT clearColor[4] = { 0.32f, 0.32f, 0.32f, 1.f };
					D3D12_CLEAR_VALUE clrValue{};
					clrValue.Format = FormatToDXFormat(m_FBInfo.imageFormats[j]);
					clrValue.Color[0] = clearColor[0];
					clrValue.Color[1] = clearColor[1];
					clrValue.Color[2] = clearColor[2];
					clrValue.Color[3] = clearColor[3];
					TN_DX_CHECK(device->CreateCommittedResource(
						&HeapProps,
						D3D12_HEAP_FLAG_NONE,
						&texDesc,
						D3D12_RESOURCE_STATE_RENDER_TARGET, 
						&clrValue,
						IID_PPV_ARGS(m_RenderTargets[i][j].ReleaseAndGetAddressOf())
					));

					m_RenderTargets[i][j]->SetName(L"FrameBuffer");

					device->CreateRenderTargetView(m_RenderTargets[i][j].Get(), nullptr, rtvHandle);

					rtvHandle.Offset(1, m_RtvDescriptorSize);
				}
				else
				{
					D3D12_DESCRIPTOR_HEAP_DESC DsvHeapDesc = {};
					DsvHeapDesc.NumDescriptors = g_FrameCount;
					DsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

					DsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
					TN_DX_CHECK(GraphicsContext::Device()->CreateDescriptorHeap(&DsvHeapDesc, IID_PPV_ARGS(m_DsvDescriptorHeap.GetAddressOf())));

					CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_DsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
					m_DvsDescriptorSize = GraphicsContext::Device()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
					D3D12_RESOURCE_DESC texDesc = {};
					texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
					texDesc.Width = m_FBInfo.width;
					texDesc.Height = (UINT)m_FBInfo.height;
					texDesc.DepthOrArraySize = 1;
					texDesc.MipLevels = 1;
					texDesc.Format = FormatToDXFormat(m_FBInfo.imageFormats[j]);
					texDesc.SampleDesc.Count = 1;
					texDesc.SampleDesc.Quality = 0;
					texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
					texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

					D3D12_HEAP_PROPERTIES HeapProps;
					HeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
					HeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
					HeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
					HeapProps.CreationNodeMask = 1;
					HeapProps.VisibleNodeMask = 1;

					D3D12_CLEAR_VALUE clrValue{};
					clrValue.Format = FormatToDXFormat(m_FBInfo.imageFormats[j]);
					clrValue.DepthStencil.Depth = 1.0f;
					clrValue.DepthStencil.Stencil = 0;

					TN_DX_CHECK(device->CreateCommittedResource(
						&HeapProps,
						D3D12_HEAP_FLAG_NONE,
						&texDesc,
						D3D12_RESOURCE_STATE_DEPTH_WRITE,
						&clrValue,
						IID_PPV_ARGS(m_DepthTarget[i].ReleaseAndGetAddressOf())
					));

					m_DepthTarget[i]->SetName(L"FrameBuffer");
					device->CreateDepthStencilView(m_DepthTarget[i].Get(), nullptr, dsvHandle);
				}
			}
		}
	}
}