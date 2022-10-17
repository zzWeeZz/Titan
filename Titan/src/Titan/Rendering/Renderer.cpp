#include "TNpch.h"
#include "Renderer.h"

#include "Titan/Application.h"

#include <glm/gtx/transform.hpp>

#include "Titan/Rendering/GraphicsContext.h"
#include "Titan/Rendering/Pipeline/Pipeline.h"
#include <Titan/Rendering/Buffers/VertexBuffer.h>
#include <Titan/Rendering/Buffers/IndexBuffer.h>
#include <Titan/Rendering/Vertices.h>
#include <Titan/Rendering/Framebuffer.h>
#include <dx12helpers/d3dx12.h>
#include <Titan/Rendering/Buffers/ConstantBuffer.h>
#include "Titan/Rendering/Buffers/ConstantBuffers.h"
#include <Optick/src/optick.h>
namespace Titan
{
	struct Cache
	{
		Ref<Pipeline> StaticMeshPipeline;
		std::vector<Vertex> vertices;
		Ref<VertexBuffer> vertexBuffer;
		Ref<IndexBuffer> indexBuffer;

		CameraCmd currentCamera = {};
		std::vector<MeshCmd> meshCmds;

		Ref<Framebuffer> testFB;

		CameraData cameraData = {};
		Ref<ConstantBuffer> cameraBuffer;

		ModelData modelData = {};
		Ref<ConstantBuffer> modelBuffer;

		D3D12_VIEWPORT viewPort = {};
		D3D12_RECT rect = {};
	};
	static Scope<Cache> s_Cache = CreateScope<Cache>();
	void Renderer::Submit(const CameraCmd& cameraCmd)
	{
		s_Cache->currentCamera = cameraCmd;
	}
	void Renderer::Submit(const MeshCmd& meshCmd)
	{
		s_Cache->meshCmds.push_back(meshCmd);
	}
	void Renderer::Initialize()
	{
		InitializePipelines();

		s_Cache->cameraBuffer = ConstantBuffer::Create(sizeof(CameraData));
		s_Cache->modelBuffer = ConstantBuffer::Create(sizeof(ModelData));

		FramebufferInfo fbInfo{};
		fbInfo.height = 720;
		fbInfo.width = 1280;
		fbInfo.imageFormats = { ImageFormat::RGBA8UN, ImageFormat::Depth24 };
		s_Cache->testFB = Framebuffer::Create(fbInfo);


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
		rec.bottom = Application::GetWindow().GetHeight();
		rec.right = Application::GetWindow().GetWidth();
	}

	void Renderer::Begin()
	{
		OPTICK_GPU_CONTEXT(GraphicsContext::CommandList().Get());
		GraphicsContext::WaitForNextFrame();
		GraphicsContext::GetCurrentCommandAllocator()->Reset();
		GraphicsContext::Reset(s_Cache->StaticMeshPipeline);
		GraphicsContext::Begin();
		s_Cache->testFB->Bind();
		s_Cache->testFB->Clear();
		
		
		s_Cache->StaticMeshPipeline->Bind();
		GraphicsContext::CommandList()->RSSetViewports(1, &s_Cache->viewPort);
		GraphicsContext::CommandList()->RSSetScissorRects(1, &s_Cache->rect);
		
		/*s_Cache->cameraData.view = s_Cache->currentCamera.view;
		s_Cache->cameraData.proj = s_Cache->currentCamera.proj;
		s_Cache->cameraBuffer->SetData(&s_Cache->cameraData, sizeof(CameraData));
		s_Cache->cameraBuffer->Bind(0);*/
		OPTICK_GPU_EVENT("Draw");
		size_t i = 0;
		for (auto& cmd : s_Cache->meshCmds)
		{
			cmd.package.vertexBuffer->Bind();
			cmd.package.indexBuffer->Bind();
			s_Cache->cameraData.view = s_Cache->currentCamera.view;
			s_Cache->cameraData.proj = s_Cache->currentCamera.proj;
			s_Cache->cameraData.mdlSpace = cmd.transform;
			s_Cache->cameraBuffer->SetData(&s_Cache->cameraData, sizeof(CameraData), i);
			s_Cache->cameraBuffer->Bind(i, sizeof(CameraData));
			OPTICK_GPU_EVENT("DrawIndexedInstanced");
			GraphicsContext::CommandList()->DrawIndexedInstanced(cmd.package.indexBuffer->GetIndexCount(), 1, 0, 0, 0);
			i++;
		}
		
		
		s_Cache->meshCmds.clear();
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
		OPTICK_GPU_FLIP(GraphicsContext::SwapChain().Get());
		OPTICK_CATEGORY("Present", Optick::Category::Wait);
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
		s_Cache->StaticMeshPipeline = Pipeline::Create(info);
	}
}
