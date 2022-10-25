//#pragma once
//#include <dxil/include/dxcapi.h>
//#include "Titan/Core/TitanFormats.h"
//#include <d3dcompiler.h>
//
//namespace Titan
//{
//	class ShaderCompiler
//	{
//	public:
//		ShaderCompiler(const std::filesystem::path& shaderPath, ShaderStage stage);
//		void GetShaderFile(std::string& outShaderFile);
//		bool Compile(std::string& errorMessage);
//		void GetInputLayout(std::vector<D3D12_INPUT_ELEMENT_DESC>& outInputLayout);
//		void GetRootParameters(std::vector<D3D12_ROOT_PARAMETER>& outRootPrameters);
//		//void GetSamplers();
//		WinRef<IDxcBlob> GetBlob();
//		void GetReflector(ID3D12ShaderReflection** outReflector);
//		~ShaderCompiler() = default;
//	private:
//		void CheckIfCompiled();
//		ShaderStage m_ShaderStage;
//		std::filesystem::path m_ShaderPath;
//		std::string m_ShaderFile;
//		WinRef<IDxcResult> m_Result;
//		WinRef<IDxcUtils> pUtils;
//		WinRef<IDxcBlobEncoding> m_Blob;
//	};
//}
