#pragma once
#include <d3d12.h>
#include "Titan/Core/TitanMemory.h"
#include "Titan/Core/TitanFormats.h"
namespace Titan
{
	struct PipelineInfo
	{
		std::filesystem::path vsPath = "";
		std::filesystem::path psPath = "";

		DepthState depthState;
		CullState depthCullState;
	};
	class Pipeline
	{
		friend class GraphicsContext;
	public:
		Pipeline(const PipelineInfo& info);
		void Bind();
		static Ref<Pipeline> Create(const PipelineInfo& info);
	private:
		D3D12_DEPTH_STENCIL_DESC GetDepthStencilDesc(const DepthState& state, const CullState& depthCull);
		WinRef<ID3D12PipelineState> m_PipelineStateObject; // pso containing a pipeline state

		WinRef<ID3D12RootSignature> m_RootSignature; // root signature defines data shaders will access
	};
}
