#include "TNpch.h"
//#include "ShaderCompiler.h"
//namespace Titan
//{
//	ShaderCompiler::ShaderCompiler(const std::filesystem::path& shaderPath, ShaderStage stage)
//	{
//		if (!std::filesystem::exists(shaderPath))
//		{
//			TN_CORE_ERROR("Could not find file {0}!", shaderPath.string());
//			return;
//		}
//
//		if (shaderPath.extension() != ".hlsl")
//		{
//			TN_CORE_ERROR("file {0} is not a hlsl file!", shaderPath.string());
//			return;
//		}
//		std::ifstream fin(shaderPath, std::ios::binary | std::ios::ate);
//		fin.seekg(0, std::ios::end);
//		m_ShaderFile.resize(fin.tellg());
//		fin.seekg(0, std::ios::beg);
//		fin.read(m_ShaderFile.data(), m_ShaderFile.size());
//
//		TN_DX_CHECK(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(pUtils.GetAddressOf())));
//		m_ShaderPath = shaderPath;
//		m_ShaderStage = stage;
//	}
//	void ShaderCompiler::GetShaderFile(std::string& outShaderFile)
//	{
//		outShaderFile = m_ShaderFile;
//	}
//	bool ShaderCompiler::Compile(std::string& errorMessage)
//	{
//		WinRef<IDxcCompiler3> compiler;
//		TN_DX_CHECK(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(compiler.GetAddressOf())));
//		pUtils->CreateBlob(m_ShaderFile.c_str(), static_cast<UINT>(m_ShaderFile.size()), CP_UTF8, m_Blob.GetAddressOf());
//
//
//		DxcBuffer sourceBuffer;
//		sourceBuffer.Ptr = m_Blob->GetBufferPointer();
//		sourceBuffer.Size = m_Blob->GetBufferSize();
//		sourceBuffer.Encoding = 0;
//
//		std::vector<LPCWSTR> arguments;
//		//-E for the entry point (eg. PSMain)
//		arguments.push_back(L"-E");
//		arguments.push_back(L"main");
//
//		//-T for the target profile (eg. ps_6_2)
//		arguments.push_back(L"-T");
//		switch (m_ShaderStage)
//		{
//		case Titan::ShaderStage::VS:
//			arguments.push_back(L"vs_6_2");
//			break;
//		case Titan::ShaderStage::PS:
//			arguments.push_back(L"ps_6_2");
//			break;
//		case Titan::ShaderStage::GS:
//			arguments.push_back(L"gs_6_2");
//			break;
//		case Titan::ShaderStage::DS:
//			arguments.push_back(L"ds_6_2");
//			break;
//		case Titan::ShaderStage::HS:
//			arguments.push_back(L"hs_6_2");
//			break;
//		case Titan::ShaderStage::CS:
//			arguments.push_back(L"cs_6_2");
//			break;
//		case Titan::ShaderStage::MS:
//			arguments.push_back(L"ms_6_2");
//			break;
//		case Titan::ShaderStage::AS:
//			arguments.push_back(L"as_6_2");
//			break;
//		default:
//			break;
//		}
//
//		//Strip reflection data and pdbs (see later)
//		arguments.push_back(L"-Qstrip_debug");
//
//
//		arguments.push_back(DXC_ARG_WARNINGS_ARE_ERRORS); //-WX
//		arguments.push_back(DXC_ARG_DEBUG); //-Zi
//		arguments.push_back(DXC_ARG_PACK_MATRIX_ROW_MAJOR); //-Zp
//
//		compiler->Compile(&sourceBuffer, arguments.data(), static_cast<UINT>(arguments.size()), nullptr, IID_PPV_ARGS(m_Result.GetAddressOf()));
//
//		WinRef<IDxcBlobUtf8> pErrors;
//		m_Result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(pErrors.GetAddressOf()), nullptr);
//		if (pErrors && pErrors->GetStringLength() > 0)
//		{
//			errorMessage = (char*)pErrors->GetBufferPointer();
//			return false;
//		}
//		errorMessage = "";
//
//		return true;
//	}
//	void ShaderCompiler::GetInputLayout(std::vector<D3D12_INPUT_ELEMENT_DESC>& inputLayout)
//	{
//		CheckIfCompiled();
//		ID3D12ShaderReflection* pReflector = nullptr;
//		GetReflector(&pReflector);
//		D3D12_SHADER_DESC decs;
//		pReflector->GetDesc(&decs);
//
//		std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayoutDesc;
//		for (UINT i = 0; i < decs.InputParameters; i++)
//		{
//			D3D12_SIGNATURE_PARAMETER_DESC paramDesc;
//			pReflector->GetInputParameterDesc(i, &paramDesc);
//			std::string f3C(paramDesc.SemanticName);
//			f3C = f3C.substr(0, 3);
//			if (f3C == "SV_")
//			{
//				continue;
//			}
//			// Fill out input element desc
//			D3D12_INPUT_ELEMENT_DESC elementDesc;
//			elementDesc.SemanticName = paramDesc.SemanticName;
//			elementDesc.SemanticIndex = paramDesc.SemanticIndex;
//			elementDesc.InputSlot = 0;
//			elementDesc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
//			elementDesc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
//			elementDesc.InstanceDataStepRate = 0;
//			// determine DXGI format
//			if (paramDesc.Mask == 1)
//			{
//				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
//					elementDesc.Format = DXGI_FORMAT_R32_UINT;
//				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
//					elementDesc.Format = DXGI_FORMAT_R32_SINT;
//				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
//					elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
//			}
//			else if (paramDesc.Mask <= 3)
//			{
//				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
//					elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
//				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
//					elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
//				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
//					elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
//			}
//			else if (paramDesc.Mask <= 7)
//			{
//				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
//					elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
//				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
//					elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
//				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
//					elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
//			}
//			else if (paramDesc.Mask <= 15)
//			{
//				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
//					elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
//				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
//					elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
//				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
//					elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
//			}
//
//			// Save element desc
//			inputLayout.push_back(elementDesc);
//		}
//	}
//
//	void ShaderCompiler::GetRootParameters(std::vector<D3D12_ROOT_PARAMETER>& outRootPrameters)
//	{
//		CheckIfCompiled();
//		ID3D12ShaderReflection* pReflector = nullptr;
//		GetReflector(&pReflector);
//		D3D12_SHADER_DESC desc;
//		pReflector->GetDesc(&desc);
//
//		for (size_t i = 0; i < desc.BoundResources; ++i)
//		{
//			D3D12_SHADER_INPUT_BIND_DESC inputBinDesc;
//			pReflector->GetResourceBindingDesc(static_cast<UINT>(i), &inputBinDesc);
//			if (inputBinDesc.Type == D3D10_SIT_CBUFFER)
//			{
//				auto& range = outRootPrameters.emplace_back();
//				range.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
//				range.Descriptor = { inputBinDesc.BindPoint, inputBinDesc.Space };
//				range.ShaderVisibility = static_cast<D3D12_SHADER_VISIBILITY>(m_ShaderStage);
//			}
//			else if (inputBinDesc.Type == D3D10_SIT_TEXTURE)
//			{
//				D3D12_DESCRIPTOR_RANGE range{};
//				range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
//				range.RegisterSpace = inputBinDesc.Space;
//				range.NumDescriptors = 1;
//				range.BaseShaderRegister = inputBinDesc.BindPoint;
//				range.OffsetInDescriptorsFromTableStart = D3D12_APPEND_ALIGNED_ELEMENT;
//
//				D3D12_ROOT_DESCRIPTOR_TABLE table{};
//				table.NumDescriptorRanges = 1;
//				table.pDescriptorRanges = &range;
//
//				auto& param = outRootPrameters.emplace_back();
//				param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
//				param.DescriptorTable = table;
//				param.ShaderVisibility = static_cast<D3D12_SHADER_VISIBILITY>(m_ShaderStage);
//			}
//		}
//	}
//
//	WinRef<IDxcBlob> ShaderCompiler::GetBlob()
//	{
//		WinRef<IDxcBlob> blob;
//		m_Result->GetResult(blob.GetAddressOf());
//		return blob;
//	}
//
//	void ShaderCompiler::GetReflector(ID3D12ShaderReflection** reflector)
//	{
//		CheckIfCompiled();
//		WinRef<IDxcBlob> pReflectionData;
//		m_Result->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(pReflectionData.GetAddressOf()), nullptr);
//		DxcBuffer reflectionBuffer;
//		reflectionBuffer.Ptr = pReflectionData->GetBufferPointer();
//		reflectionBuffer.Size = pReflectionData->GetBufferSize();
//		reflectionBuffer.Encoding = 0;
//		pUtils->CreateReflection(&reflectionBuffer, IID_PPV_ARGS(reflector));
//	}
//
//	void ShaderCompiler::CheckIfCompiled()
//	{
//		TN_CORE_ASSERT(m_Result, "You need to compile shader before you try accessing any of the data!");
//	}
//}