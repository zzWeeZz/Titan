#include "TNpch.h"
#include "Renderer.h"

#include "Titan/Application.h"

#include <glm/gtx/transform.hpp>

#include "Titan/Rendering/GraphicsContext.h"
#include "Titan/Rendering/Pipeline.h"
#include <Titan/Rendering/Buffers/VertexBuffer.h>
#include <Titan/Rendering/Buffers/IndexBuffer.h>
#include <Titan/Rendering/Vertices.h>
#include <Titan/Rendering/Framebuffer.h>
#include <dx12helpers/d3dx12.h>
#include <Titan/Rendering/Buffers/ConstantBuffer.h>
#include "Titan/Rendering/Buffers/ConstantBuffers.h"
namespace Titan
{
	struct Cache
	{
		Ref<Pipeline> TrianglePipeline;
		std::vector<Vertex> vertices;
		Ref<VertexBuffer> vertexBuffer;
		Ref<IndexBuffer> indexBuffer;

		Ref<Framebuffer> testFB;

		CameraData cameraData;
		Ref<ConstantBuffer> cbuffer;

		D3D12_VIEWPORT viewPort;
		D3D12_RECT rect;
	};
	static Scope<Cache> s_Cache = CreateScope<Cache>();
	void Renderer::Initialize()
	{
		InitializePipelines();

		s_Cache->cbuffer = ConstantBuffer::Create();

		s_Cache->vertices.push_back({ {-0.5,0.5,0.5,1},{1,0,0,1} });
		s_Cache->vertices.push_back({ {0.5,-0.5,0.5,1},{0,1,0,1} });
		s_Cache->vertices.push_back({ {-0.5,-0.5,0.5,1},{0,0,1,1} });
		s_Cache->vertices.push_back({ {0.5,0.5,0.5,1},{1,0,1,1} });

		s_Cache->vertices.push_back({ {-1.5,1.5,0.7,1},{1,1,1,1} });
		s_Cache->vertices.push_back({ {1.5,-1.5,0.7,1},{1,1,1,1} });
		s_Cache->vertices.push_back({ {-1.5,-1.5,0.7,1},{1,1,1,1} });
		s_Cache->vertices.push_back({ {1.5,1.5,0.7,1},{1,1,1,1} });
		VertexBufferInfo info{};
		info.sizeOfData = s_Cache->vertices.size();
		info.vertexData = s_Cache->vertices.data();
		info.sizeOfVertex = sizeof(Vertex);
		info.debugName = L"Vertex";
		s_Cache->vertexBuffer = VertexBuffer::Create(info);

		FramebufferInfo fbInfo{};
		fbInfo.height = 720;
		fbInfo.width = 1280;
		fbInfo.imageFormats = { ImageFormat::RGBA8UN, ImageFormat::Depth24 };
		s_Cache->testFB = Framebuffer::Create(fbInfo);


		std::vector<DWORD> indices = {
			0,1,2,
			0,3,1,
			4,5,6,
			4,7,5
		};
		IndexBufferInfo iInfo{};
		iInfo.indexData = indices.data();
		iInfo.sizeOfArray = indices.size();
		s_Cache->indexBuffer = IndexBuffer::Create(iInfo);

		auto& view =  s_Cache->viewPort;
		view.Height = static_cast<float>(Application::GetWindow().GetHeight());
		view.Width = static_cast<float>(Application::GetWindow().GetWidth());
		view.MinDepth = 0.0f;
		view.MaxDepth = 1.0f;
		view.TopLeftX = 0;
		view.TopLeftY = 0;

		auto& rec = s_Cache->rect;
		rec.left = 0;
		rec.top = 0;
		rec.right = Application::GetWindow().GetHeight();
		rec.bottom = Application::GetWindow().GetWidth();
	}

	void Renderer::Begin()
	{
		GraphicsContext::WaitForNextFrame();
		GraphicsContext::GetCurrentCommandAllocator()->Reset();
		GraphicsContext::Reset(s_Cache->TrianglePipeline);
		GraphicsContext::Begin();
		s_Cache->testFB->Bind();
		s_Cache->testFB->Clear();
		
		s_Cache->cbuffer->SetData(&s_Cache->cameraData, sizeof(CameraData));
		s_Cache->TrianglePipeline->Bind();
		GraphicsContext::CommandList()->RSSetViewports(1, &s_Cache->viewPort);
		GraphicsContext::CommandList()->RSSetScissorRects(1, &s_Cache->rect);
		s_Cache->vertexBuffer->Bind();
		s_Cache->indexBuffer->Bind();
		s_Cache->cbuffer->Bind(0);
		GraphicsContext::CommandList()->DrawIndexedInstanced(12, 1, 0,0,0);
		
		CopyResource(GraphicsContext::GetCurrentRtv().Get(), s_Cache->testFB->GetCurrentRtv().Get());
	}

	void Renderer::End()
	{
		GraphicsContext::End();
	}

	void Renderer::DrawCommands()
	{
		ID3D12CommandList* ppCommandList[] = { GraphicsContext::CommandList().Get() };
		GraphicsContext::CommandQueue()->ExecuteCommandLists(_countof(ppCommandList), ppCommandList);

		GraphicsContext::SignalCommandQueue();

		GraphicsContext::SwapChain()->Present(0, 0);
	}

	void Renderer::Shutdown()
	{
		GraphicsContext::Shutdown();
	}
	void Renderer::CopyResource(ID3D12Resource* dest, ID3D12Resource* source)
	{
		{
			auto CopyRB = CD3DX12_RESOURCE_BARRIER::Transition(
				source,
				D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATE_COPY_SOURCE);
			auto dcsRB = CD3DX12_RESOURCE_BARRIER::Transition(
				dest,
				D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATE_COPY_DEST);
			std::array<CD3DX12_RESOURCE_BARRIER, 2> arr = { CopyRB, dcsRB };
			GraphicsContext::CommandList()->ResourceBarrier(
				arr.size(),
				arr.data());
		}
		GraphicsContext::CommandList()->CopyResource(dest, source);
		{
			auto CopyRB = CD3DX12_RESOURCE_BARRIER::Transition(
				source,
				D3D12_RESOURCE_STATE_COPY_SOURCE,
				D3D12_RESOURCE_STATE_RENDER_TARGET);
			auto dcsRB = CD3DX12_RESOURCE_BARRIER::Transition(
				dest,
				D3D12_RESOURCE_STATE_COPY_DEST,
				D3D12_RESOURCE_STATE_RENDER_TARGET);
			std::array<CD3DX12_RESOURCE_BARRIER, 2> arr = { CopyRB, dcsRB };
			GraphicsContext::CommandList()->ResourceBarrier(
				arr.size(),
				arr.data());
		}
	}
	void Renderer::InitializePipelines()
	{
		PipelineInfo info{};
		info.vsPath = "Engine/Shaders/triangle_vs.hlsl";
		info.psPath = "Engine/Shaders/triangle_ps.hlsl";
		info.depthState = DepthState::ReadWrite;
		info.depthCullState = CullState::None;
		s_Cache->TrianglePipeline = Pipeline::Create(info);
	}
}
