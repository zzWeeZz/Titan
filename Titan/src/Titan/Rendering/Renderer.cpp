#include "TNpch.h"
#include "Renderer.h"

#include "Titan/Application.h"

#include <glm/gtx/transform.hpp>

#include "Titan/Rendering/GraphicsContext.h"
#include "Titan/Rendering/Pipeline.h"
#include <Titan/Rendering/Buffers/VertexBuffer.h>
#include <Titan/Rendering/Buffers/IndexBuffer.h>
#include <Titan/Rendering/Vertices.h>

namespace Titan
{
	struct Cache
	{
		Ref<Pipeline> TrianglePipeline;
		std::vector<Vertex> vertices;
		Ref<VertexBuffer> vertexBuffer;
		Ref<IndexBuffer> indexBuffer;

		D3D12_VIEWPORT viewPort;
		D3D12_RECT rect;
	};
	static Scope<Cache> s_Cache = CreateScope<Cache>();
	void Renderer::Initialize()
	{
		InitializePipelines();
		s_Cache->vertices.push_back({ {-0.5,0.5,0.5,1},{1,0,0,1} });
		s_Cache->vertices.push_back({ {0.5,-0.5,0.5,1},{0,1,0,1} });
		s_Cache->vertices.push_back({ {-0.5,-0.5,0.5,1},{0,0,1,1} });
		s_Cache->vertices.push_back({ {0.5,0.5,0.5,1},{1,0,1,1} });
		VertexBufferInfo info{};
		info.sizeOfData = s_Cache->vertices.size();
		info.vertexData = s_Cache->vertices.data();
		info.sizeOfVertex = sizeof(Vertex);
		info.debugName = L"Vertex";
		s_Cache->vertexBuffer = VertexBuffer::Create(info);

		std::vector<DWORD> indices = { 0,1,2,0,3,1 };
		IndexBufferInfo iInfo{};
		iInfo.indexData = indices.data();
		iInfo.sizeOfArray = indices.size();
		s_Cache->indexBuffer = IndexBuffer::Create(iInfo);

		auto& view =  s_Cache->viewPort;
		view.Height = Application::GetWindow().GetHeight();
		view.Width = Application::GetWindow().GetWidth();
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

		s_Cache->TrianglePipeline->Bind();
		GraphicsContext::CommandList()->RSSetViewports(1, &s_Cache->viewPort);
		GraphicsContext::CommandList()->RSSetScissorRects(1, &s_Cache->rect);
		s_Cache->vertexBuffer->Bind();
		s_Cache->indexBuffer->Bind();
		GraphicsContext::CommandList()->DrawIndexedInstanced(6, 1, 0,0,0);
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
	void Renderer::InitializePipelines()
	{
		PipelineInfo info{};
		info.vsPath = "Engine/Shaders/triangle_vs.hlsl";
		info.psPath = "Engine/Shaders/triangle_ps.hlsl";
		s_Cache->TrianglePipeline = Pipeline::Create(info);
	}
}
