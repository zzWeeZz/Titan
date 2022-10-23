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
		std::vector<Vertex> vertices;
		

		CameraCmd currentCamera = {};
		std::vector<MeshCmd> meshCmds;

		Ref<Framebuffer> testFB;
		Ref<Pipeline> pipeline;
		CameraData cameraData = {};
		Ref<ConstantBuffer> cameraBuffer;

		ModelData modelData = {};
		Ref<ConstantBuffer> modelBuffer;

	};
	static Scope<Cache> s_Cache = CreateScope<Cache>();
	void Renderer::Submit(const CameraCmd& cameraCmd)
	{
		s_Cache->currentCamera = cameraCmd;
	}
	void Renderer::Submit(const MeshCmd& meshCmd)
	{
	}
	void Renderer::Initialize()
	{
		PipelineInfo info{};
		info.vsPath = "Engine/Shaders/triangle_vs.spv";
		info.psPath = "Engine/Shaders/triangle_fs.spv";
		s_Cache->pipeline = Pipeline::Create(info);
	}

	void Renderer::Begin()
	{
	}

	void Renderer::End()
	{
	}

	void Renderer::DrawCommands()
	{
	
	}

	void Renderer::Shutdown()
	{
	}
}
