#include "TNpch.h"
#include "GLSLImporter.h"
#include <shaderc/shaderc.hpp>
#include <SPIRV-Reflect/spirv_reflect.h>
#include "Titan/Utils/FilesystemUtils.h"
namespace Titan
{
	void GLSLImporter::Import(const std::filesystem::path& path, Shader& shader)
	{
		shader = Compile(path);
	}

	Shader GLSLImporter::Compile(const std::filesystem::path& path)
	{
		TN_CORE_INFO("Compiling Glsl file: {0}... ", path.filename().string());
		std::filesystem::path binaryPath = FilesystemUtils::TempEntry;
		binaryPath /= "shaders";


		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		Shader shader{};
#ifdef TN_CONFIG_DEBUG
		options.SetOptimizationLevel(shaderc_optimization_level_zero);
		options.SetGenerateDebugInfo();
		binaryPath /= "OptiZero";
#else
		options.SetOptimizationLevel(shaderc_optimization_level_performance);
		binaryPath /= "OptiPerformace";
#endif // TN_CONFIG_DEBUG


		binaryPath /= path.stem();
		binaryPath += "_sprv_assembly.tns";
		// Get out what shadertype the shader is
		shader.m_ShaderType = GetShaderTypeFromFile(path);
	/*	if (std::filesystem::exists(binaryPath))
		{
			shader.spvAssembly = ReadBinary(binaryPath);
			auto asmSprv = compiler.AssembleToSpv(shader.spvAssembly);
			if (asmSprv.GetCompilationStatus() == shaderc_compilation_status_success)
			{
				shader.spvBinary = { asmSprv.cbegin(), asmSprv.cend() };
				Reflect(shader);
				return shader;
			}
		}*/
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);

		// Get the whole file to string.
		std::ifstream fin(path, std::ios::binary | std::ios::ate);
		fin.seekg(0, std::ios::end);
		std::string file;
		file.resize(fin.tellg());
		fin.seekg(0, std::ios::beg);
		fin.read(file.data(), file.size());



		auto preprocess = compiler.PreprocessGlsl(file, (shaderc_shader_kind)shader.m_ShaderType, path.stem().string().c_str(), options);

		TN_CORE_ASSERT(preprocess.GetCompilationStatus() == shaderc_compilation_status_success, preprocess.GetErrorMessage().c_str());

		shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(preprocess.begin(), (shaderc_shader_kind)shader.m_ShaderType, path.stem().string().c_str(), options);

		TN_CORE_ASSERT(result.GetCompilationStatus() == shaderc_compilation_status_success, result.GetErrorMessage().c_str());
		shader.m_Spirv = { result.cbegin(), result.cend() };

		shader.Reflect();

		return shader;
	}
}