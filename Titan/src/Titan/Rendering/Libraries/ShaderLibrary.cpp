#include "TNpch.h"
#include "ShaderLibrary.h"
#include "shaderc/shaderc.hpp"
namespace Titan
{
	Shader& ShaderLibrary::Get(const std::filesystem::path& path)
	{
		if (s_Library.contains(path.filename()))
		{
			return s_Library[path.filename()];
		}

		s_Library[path.filename()] = Compile(path);
		return s_Library[path.filename()];
	}
	Shader ShaderLibrary::Compile(const std::filesystem::path& path)
	{
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		Shader shader{};
#ifdef TN_CONFIG_DEBUG
		options.SetOptimizationLevel(shaderc_optimization_level_zero);
#else
		options.SetOptimizationLevel(shaderc_optimization_level_performance);
#endif // TN_CONFIG_DEBUG
		
		// Get the whole file to string.
		std::ifstream fin(path, std::ios::binary | std::ios::ate);
		fin.seekg(0, std::ios::end);
		shader.shaderFile.resize(fin.tellg());
		fin.seekg(0, std::ios::beg);
		fin.read(shader.shaderFile.data(), shader.shaderFile.size());
		// Get out what shadertype the shader is
		auto extension = path.extension().string();
		if (extension == ".vert")
		{
			shader.shaderType = ShaderType::Vertex;
		}
		else if (extension == ".frag")
		{
			shader.shaderType = ShaderType::Fragment;
		}
		else if (extension == ".comp")
		{
			shader.shaderType = ShaderType::Compute;
		}
		else
		{
			TN_CORE_ASSERT(false, "could not find shadertype, can't continue compling");
		}

		auto preprocess = compiler.PreprocessGlsl(shader.shaderFile, (shaderc_shader_kind)shader.shaderType, path.stem().string().c_str(), options);

		TN_CORE_ASSERT(preprocess.GetCompilationStatus() == shaderc_compilation_status_success, preprocess.GetErrorMessage().c_str());

		shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(preprocess.begin(), (shaderc_shader_kind)shader.shaderType, path.stem().string().c_str(), options);

		TN_CORE_ASSERT(result.GetCompilationStatus() == shaderc_compilation_status_success, result.GetErrorMessage().c_str());
		shader.spvBinary = { result.cbegin(), result.cend() };
		return shader;
	}
}