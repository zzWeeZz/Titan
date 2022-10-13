#include "TNpch.h"
#include "Pipeline.h"
#include <dx12helpers/d3dx12.h>
#include "Titan/Rendering/GraphicsContext.h"
#include <Titan/Core/TitanFormats.h>
namespace Titan
{
	Pipeline::Pipeline(const PipelineInfo& info)
	{
		D3D12_DESCRIPTOR_RANGE ranges[1] = {};
		ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		ranges[0].NumDescriptors = 1;
		ranges[0].BaseShaderRegister = 0; // this is the bind point;
		ranges[0].RegisterSpace = 0;
		ranges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		D3D12_ROOT_DESCRIPTOR_TABLE table{};
		table.NumDescriptorRanges = _countof(ranges);
		table.pDescriptorRanges = &ranges[0];

		D3D12_ROOT_PARAMETER parameters[1];
		parameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		parameters[0].DescriptorTable = table;
		parameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init(_countof(parameters),
			parameters,
			0,
			nullptr,
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | // we can deny shader stages here for better performance
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS);
		ID3DBlob* signature;
		TN_DX_CHECK(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr));

		TN_DX_CHECK(GraphicsContext::Device()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(m_RootSignature.GetAddressOf())));

		ID3DBlob* vertexShader;
		ID3DBlob* errorBuffer;
		TN_DX_CHECK(D3DCompileFromFile(
			info.vsPath.wstring().c_str(),
			nullptr,
			nullptr,
			"main",
			"vs_5_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
			0,
			&vertexShader,
			&errorBuffer));

		ID3DBlob* pxShader = nullptr;

		TN_DX_CHECK(D3DCompileFromFile(
			info.psPath.wstring().c_str(),
			nullptr,
			nullptr,
			"main",
			"ps_5_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
			0,
			&pxShader,
			&errorBuffer));
		if (errorBuffer)
		{
			TN_CORE_ERROR(errorBuffer->GetBufferPointer());
		}
		D3D12_SHADER_BYTECODE vsByteCode;
		vsByteCode.BytecodeLength = vertexShader->GetBufferSize();
		vsByteCode.pShaderBytecode = vertexShader->GetBufferPointer();
		D3D12_SHADER_BYTECODE psByteCode;
		psByteCode.BytecodeLength = pxShader->GetBufferSize();
		psByteCode.pShaderBytecode = pxShader->GetBufferPointer();

		std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayoutDesc;
		GetInputLayoutFromBlob(vertexShader, inputLayoutDesc);
		D3D12_INPUT_LAYOUT_DESC inputLayout;
		inputLayout.NumElements = static_cast<UINT>(inputLayoutDesc.size());
		inputLayout.pInputElementDescs = inputLayoutDesc.data();

		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
		pipelineDesc.VS = vsByteCode;
		pipelineDesc.PS = psByteCode;
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

	void Pipeline::GetInputLayoutFromBlob(ID3DBlob* blob, std::vector<D3D12_INPUT_ELEMENT_DESC>& InputLayout)
	{
		ID3D12ShaderReflection* pReflector = nullptr;
		TN_DX_CHECK(D3DReflect(blob->GetBufferPointer(), blob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&pReflector));
		D3D12_SHADER_DESC decs;
		pReflector->GetDesc(&decs);

		std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayoutDesc;
		/*LOGINFO("Found {} input descriptions...", decs.InputParameters);*/
		for (UINT i = 0; i < decs.InputParameters; i++)
		{
			D3D12_SIGNATURE_PARAMETER_DESC paramDesc;
			pReflector->GetInputParameterDesc(i, &paramDesc);
			std::string f3C(paramDesc.SemanticName);
			f3C = f3C.substr(0, 3);
			if (f3C == "SV_")
			{
				/*LOGINFO("Found 'SV_' ignoring {}", paramDesc.SemanticName);*/
				continue;
			}
			// Fill out input element desc
			D3D12_INPUT_ELEMENT_DESC elementDesc;
			elementDesc.SemanticName = paramDesc.SemanticName;
			elementDesc.SemanticIndex = paramDesc.SemanticIndex;
			elementDesc.InputSlot = 0;
			elementDesc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
			elementDesc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			elementDesc.InstanceDataStepRate = 0;
			// determine DXGI format
			if (paramDesc.Mask == 1)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
					elementDesc.Format = DXGI_FORMAT_R32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
					elementDesc.Format = DXGI_FORMAT_R32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
					elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
			}
			else if (paramDesc.Mask <= 3)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
					elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
					elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
					elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
			}
			else if (paramDesc.Mask <= 7)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
					elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
					elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
					elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			else if (paramDesc.Mask <= 15)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
					elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
					elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
					elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}
			/*LOGINFO("Found Semantic: {}", paramDesc.SemanticName);*/

			// Save element desc
			InputLayout.push_back(elementDesc);
		}
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