#include "TNpch.h"
#include "Pipeline.h"
#include <dx12helpers/d3dx12.h>
#include "Titan/Rendering/GraphicsContext.h"
namespace Titan
{
	Pipeline::Pipeline(const PipelineInfo& info)
	{
		CD3DX12_ROOT_SIGNATURE_DESC rootSigdesc{};
		rootSigdesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		ID3DBlob* signature;
		TN_DX_CHECK(D3D12SerializeRootSignature(&rootSigdesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr));

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

		D3D12_INPUT_LAYOUT_DESC inputLayout;
		GetInputLayoutFromBlob(vertexShader, inputLayout);

	}
	Ref<Pipeline> Pipeline::Create(const PipelineInfo& info)
	{
		return CreateRef<Pipeline>(info);
	}

	void Pipeline::GetInputLayoutFromBlob(ID3DBlob* blob, D3D12_INPUT_LAYOUT_DESC& InputLayout)
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
			inputLayoutDesc.push_back(elementDesc);
		}
		InputLayout.NumElements = inputLayoutDesc.size();
		InputLayout.pInputElementDescs = inputLayoutDesc.data();
	}
}