#include "TNpch.h"
#include "Pipeline.h"
#include <dx12helpers/d3dx12.h>
#include "Titan/Rendering/GraphicsContext.h"
#include <Titan/Core/TitanFormats.h>
#include <dxcapi.h>
namespace Titan
{
	Pipeline::Pipeline(const PipelineInfo& info)
	{
		IDxcBlobEncoding* vsSourceBlob;
		IDxcBlobEncoding* errorBuffer;

		WinRef<IDxcLibrary> library;
		TN_DX_CHECK(DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(library.GetAddressOf())));
		WinRef<IDxcCompiler> compiler;
		TN_DX_CHECK(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(compiler.GetAddressOf())));
		uint32_t codePage = CP_UTF8;
		library->CreateBlobFromFile(info.vsPath.wstring().c_str(), &codePage, &vsSourceBlob);

		WinRef<IDxcOperationResult> result;
		compiler->Compile(vsSourceBlob, info.vsPath.wstring().c_str(), L"main", L"VS_6_0", NULL, 0, NULL, 0, NULL, result.GetAddressOf());
		
		WinRef<IDxcBlob> code;
		result->GetResult(&code);


		/*(D3DCompileFromFile(
			info.vsPath.wstring().c_str(),
			nullptr,
			nullptr,
			"main",
			"vs_6_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
			0,
			&vsSourceBlob,
			&errorBuffer));*/

		ID3DBlob* pxShader = nullptr;
		/*if (errorBuffer)
		{
			TN_CORE_ERROR((char*)errorBuffer->GetBufferPointer());
			return;
		}*/
		
		errorBuffer = nullptr;
		(D3DCompileFromFile(
			info.psPath.wstring().c_str(),
			nullptr,
			nullptr,
			"main",
			"ps_6_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
			0,
			&pxShader,
			nullptr));
		/*if (errorBuffer)
		{
			TN_CORE_ERROR((char*)errorBuffer->GetBufferPointer());
			return;
		}*/
		D3D12_SHADER_BYTECODE vsByteCode;
		vsByteCode.BytecodeLength = code->GetBufferSize();
		vsByteCode.pShaderBytecode = code->GetBufferPointer();
		D3D12_SHADER_BYTECODE psByteCode;
		psByteCode.BytecodeLength = pxShader->GetBufferSize();
		psByteCode.pShaderBytecode = pxShader->GetBufferPointer();

		std::vector<D3D12_ROOT_PARAMETER> parameters;
		GetDescriptorRangesFromBlob(, parameters);


		D3D12_DESCRIPTOR_RANGE  descriptorTableRanges[1]; // only one range right now
		descriptorTableRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // this is a range of shader resource views (descriptors)
		descriptorTableRanges[0].NumDescriptors = 1; // we only have one texture right now, so the range is only 1
		descriptorTableRanges[0].BaseShaderRegister = 0; // start index of the shader registers in the range
		descriptorTableRanges[0].RegisterSpace = 0; // space 0. can usually be zero
		descriptorTableRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // this appends the range to the end of the root signature descriptor tables
		
		D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable;
		descriptorTable.NumDescriptorRanges = _countof(descriptorTableRanges); // we only have one range
		descriptorTable.pDescriptorRanges = &descriptorTableRanges[0]; // the pointer to the beginning of our ranges array
		
		auto param = parameters.emplace_back();
		param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		param.DescriptorTable = descriptorTable;
		param.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;


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
		sampler.RegisterSpace = 0;
		sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init(parameters.size(),
			parameters.data(),
			1,
			&sampler,
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | // we can deny shader stages here for better performance
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);
		ID3DBlob* signature;
		ID3DBlob* errorBlob = nullptr;
		TN_DX_CHECK(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &errorBlob));
		if (errorBlob)
		{
			TN_CORE_ERROR((char*)errorBlob->GetBufferPointer());
		}

		TN_DX_CHECK(GraphicsContext::Device()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(m_RootSignature.GetAddressOf())));

		

		std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayoutDesc;
		GetInputLayoutFromBlob(vsSourceBlob, inputLayoutDesc);
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
	void Pipeline::GetDescriptorRangesFromBlob(ID3DBlob* blob, std::vector<D3D12_ROOT_PARAMETER>& ranges)
	{
		ID3D12ShaderReflection* pReflector = nullptr;
		TN_DX_CHECK(D3DReflect(blob->GetBufferPointer(), blob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&pReflector));
		D3D12_SHADER_DESC desc;
		pReflector->GetDesc(&desc);
		
		
		for (size_t i = 0; i < desc.BoundResources; ++i)
		{
			D3D12_SHADER_INPUT_BIND_DESC inputBinDesc;
			pReflector->GetResourceBindingDesc(i, &inputBinDesc);
			if (inputBinDesc.Type == D3D10_SIT_CBUFFER)
			{
				auto& range = ranges.emplace_back();
				range.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
				range.Descriptor = { inputBinDesc.BindPoint, inputBinDesc.Space };
				range.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
			}
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