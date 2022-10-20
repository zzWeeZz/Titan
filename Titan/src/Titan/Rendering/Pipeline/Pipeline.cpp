#include "TNpch.h"
#include "Pipeline.h"
#include <dx12helpers/d3dx12.h>
#include "Titan/Rendering/GraphicsContext.h"
#include <Titan/Core/TitanFormats.h>
#include <dxil/include/dxcapi.h>
#include "Titan/Rendering/Pipeline/ShaderCompiler.h"
namespace Titan
{
	Pipeline::Pipeline(const PipelineInfo& info)
	{
		
		ShaderCompiler vsCompiler(info.vsPath, ShaderStage::VS);
		std::string errorString;
		if (!vsCompiler.Compile(errorString))
		{
			TN_CORE_ERROR(errorString);
		}

		ShaderCompiler psCompiler(info.psPath, ShaderStage::PS);
		if (!psCompiler.Compile(errorString))
		{
			TN_CORE_ERROR(errorString);
		}

		std::vector<D3D12_ROOT_PARAMETER> parameters;
		vsCompiler.GetRootParameters(parameters);
		psCompiler.GetRootParameters(parameters);

		// create a static sampler
		D3D12_STATIC_SAMPLER_DESC sampler = {};
		sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler.MipLODBias = 0;
		sampler.MaxAnisotropy = 0;
		sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		sampler.MinLOD = 0.0f;
		sampler.MaxLOD = D3D12_FLOAT32_MAX;
		sampler.ShaderRegister = 0;
		sampler.RegisterSpace = 2;
		sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init(parameters.size(),
			parameters.data(),
			1,
			&sampler,
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		ID3DBlob* signature = nullptr;
		ID3DBlob* errorBlob = nullptr;
		TN_DX_CHECK(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &signature, &errorBlob));
		if (errorBlob)
		{
			TN_CORE_ERROR((char*)errorBlob->GetBufferPointer());
		}

		TN_DX_CHECK(GraphicsContext::Device()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(m_RootSignature.GetAddressOf())));

		

		std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayoutDesc;
		vsCompiler.GetInputLayout(inputLayoutDesc);
		D3D12_INPUT_LAYOUT_DESC inputLayout;
		inputLayout.NumElements = static_cast<UINT>(inputLayoutDesc.size());
		inputLayout.pInputElementDescs = inputLayoutDesc.data();

		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
		pipelineDesc.VS = {vsCompiler.GetBlob()->GetBufferPointer(), vsCompiler.GetBlob()->GetBufferSize()};
		pipelineDesc.PS = { psCompiler.GetBlob()->GetBufferPointer(), psCompiler.GetBlob()->GetBufferSize() };
		pipelineDesc.pRootSignature = m_RootSignature.Get();
		pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineDesc.InputLayout = inputLayout;
		pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		pipelineDesc.NumRenderTargets = 1;
		pipelineDesc.SampleMask = 0xffffffff;
		pipelineDesc.SampleDesc.Count = 1;
		pipelineDesc.DSVFormat = (DXGI_FORMAT)45;
		pipelineDesc.DepthStencilState = GetDepthStencilDesc(info.depthState, info.depthCullState);

		pipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		pipelineDesc.RasterizerState.FrontCounterClockwise = TRUE;
		pipelineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

		TN_DX_CHECK(GraphicsContext::Device()->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(m_PipelineStateObject.GetAddressOf())));
		TN_CORE_INFO("Pipeline created");
	}
	void Pipeline::Bind()
	{
		GraphicsContext::CommandList()->SetPipelineState(m_PipelineStateObject.Get());
		GraphicsContext::CommandList()->SetGraphicsRootSignature(m_RootSignature.Get());
		GraphicsContext::CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
	Ref<Pipeline> Pipeline::Create(const PipelineInfo& info)
	{
		return CreateRef<Pipeline>(info);
	}

	D3D12_DEPTH_STENCIL_DESC Pipeline::GetDepthStencilDesc(const DepthState& state, const CullState& depthCull)
	{
		const D3D12_DEPTH_STENCILOP_DESC stencil = { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
		D3D12_DEPTH_STENCIL_DESC desc{};
		desc.BackFace = stencil;
		desc.FrontFace = stencil;

		desc.DepthEnable = TRUE;
		desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		desc.StencilEnable = FALSE;
		desc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
		desc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
		return desc;
	}
}