#include "TNpch.h"
#include "HLSLImporter.h"
namespace Titan
{
	void HLSLImporter::Initialize()
	{
		DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(s_Utils.GetAddressOf()));
		DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(s_Compiler.GetAddressOf()));
	}
	void HLSLImporter::Import(const std::filesystem::path& path, Shader& shader)
	{
		shader = Compile(path, shader);
	}
	Shader HLSLImporter::Compile(const std::filesystem::path& path, Shader& shader)
	{
		ShaderType type = GetShaderTypeFromFile(path);

		std::vector<LPCWSTR> arguments;
		arguments.emplace_back(L"-E");
		arguments.emplace_back(L"main");

		std::wstring target = L"";

		switch (type)
		{
		case ShaderType::Compute:
		{
			target = L"cs_6_6";
			break;
		}
		case ShaderType::Fragment:
		{
			target = L"ps_6_6";
			break;
		}
		case ShaderType::Mesh:
		{
			target = L"ms_6_6";
			break;
		}
		case ShaderType::Task:
		{
			target = L"as_6_6";
			break;
		}
		}
		Microsoft::WRL::ComPtr<IDxcBlobEncoding> sourceBlob;
		// Get the whole file to string.
		std::ifstream fin(path, std::ios::binary | std::ios::ate);
		fin.seekg(0, std::ios::end);
		std::string file;
		file.resize(fin.tellg());
		fin.seekg(0, std::ios::beg);
		fin.read(file.data(), file.size());

		s_Utils->CreateBlob(file.data(), static_cast<uint32_t>(file.size()), CP_UTF8, sourceBlob.GetAddressOf());

		arguments.push_back(L"-T");
		arguments.push_back(target.c_str());

		arguments.push_back(L"-spirv");

		arguments.push_back(DXC_ARG_WARNINGS_ARE_ERRORS);
		arguments.push_back(DXC_ARG_DEBUG);
		arguments.push_back(DXC_ARG_PACK_MATRIX_ROW_MAJOR);

		DxcBuffer sourceBuffer{};
		sourceBuffer.Ptr = sourceBlob->GetBufferPointer();
		sourceBuffer.Size = sourceBlob->GetBufferSize();
		sourceBuffer.Encoding = 0;

		Microsoft::WRL::ComPtr<IDxcResult> compileResult;
		auto hr = s_Compiler->Compile(&sourceBuffer, arguments.data(), (uint32_t)arguments.size(), nullptr, IID_PPV_ARGS(compileResult.GetAddressOf()));
		if (FAILED(hr))
		{
			TN_CORE_ASSERT(false, "HLSL File did not compile!");
		}

		Microsoft::WRL::ComPtr<IDxcBlobUtf8> pErrors;
		IDxcBlobWide* wide = nullptr;
		hr = compileResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(pErrors.GetAddressOf()), &wide);
		if (pErrors && pErrors->GetStringLength() > 0)
		{
			TN_CORE_ERROR("HLSL crashed, {}", (char*)pErrors->GetBufferPointer());
		}
		IDxcBlob* blob = nullptr;
		compileResult->GetResult(&blob);
		shader.m_ShaderType = type;
		shader.m_Spirv.resize(blob->GetBufferSize() / 4);
		memcpy(shader.m_Spirv.data(), blob->GetBufferPointer(), blob->GetBufferSize());
		shader.Reflect();
		return shader;
	}
}