#pragma once
#include "Titan/Assets/Shaders/Shader.h"

#ifdef TN_PLATFORM_WINDOWS
#include <wrl.h>
#endif
#include "dxc/dxcapi.h"

namespace Titan
{
	class HLSLImporter
	{
	public:
		static void Initialize();
		static void Import(const std::filesystem::path& path, Shader& shader);
	private:
		static Shader Compile(const std::filesystem::path& path, Shader& shader);
		static inline Microsoft::WRL::ComPtr<IDxcUtils> s_Utils;
		static inline Microsoft::WRL::ComPtr<IDxcCompiler3> s_Compiler;
	};
}