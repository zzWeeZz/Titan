#include "TNpch.h"
#include "ShaderLibrary.h"
#include "shaderc/shaderc.hpp"

#include "Titan/Utils/FilesystemUtils.h"
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
		std::filesystem::path binaryPath = FilesystemUtils::TempEntry;
		binaryPath /= "shaders";
		binaryPath /= path.stem();
		binaryPath += "_sprv_assembly.tns";

		

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		Shader shader{};
#ifdef TN_CONFIG_DEBUG
		options.SetOptimizationLevel(shaderc_optimization_level_zero);
#else
		options.SetOptimizationLevel(shaderc_optimization_level_performance);
#endif // TN_CONFIG_DEBUG
		

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
		if (std::filesystem::exists(binaryPath))
		{
			shader.spvAssembly = ReadBinary(binaryPath);
			auto asmSprv = compiler.AssembleToSpv(shader.spvAssembly);
			TN_CORE_ASSERT(asmSprv.GetCompilationStatus() == shaderc_compilation_status_success, asmSprv.GetErrorMessage().c_str());
			shader.spvBinary = {asmSprv.cbegin(), asmSprv.cend()};
			return shader;
		}

		// Get the whole file to string.
		std::ifstream fin(path, std::ios::binary | std::ios::ate);
		fin.seekg(0, std::ios::end);
		shader.shaderFile.resize(fin.tellg());
		fin.seekg(0, std::ios::beg);
		fin.read(shader.shaderFile.data(), shader.shaderFile.size());
		

		

		auto preprocess = compiler.PreprocessGlsl(shader.shaderFile, (shaderc_shader_kind)shader.shaderType, path.stem().string().c_str(), options);

		TN_CORE_ASSERT(preprocess.GetCompilationStatus() == shaderc_compilation_status_success, preprocess.GetErrorMessage().c_str());

		shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(preprocess.begin(), (shaderc_shader_kind)shader.shaderType, path.stem().string().c_str(), options);
		
		TN_CORE_ASSERT(result.GetCompilationStatus() == shaderc_compilation_status_success, result.GetErrorMessage().c_str());
		shader.spvBinary = { result.cbegin(), result.cend() };

		auto asmResult = compiler.CompileGlslToSpvAssembly(preprocess.begin(), (shaderc_shader_kind)shader.shaderType, path.stem().string().c_str(), options);

		TN_CORE_ASSERT(asmResult.GetCompilationStatus() == shaderc_compilation_status_success, asmResult.GetErrorMessage().c_str());

		shader.spvAssembly = { asmResult.cbegin(), asmResult.cend() };

		DumpBinary(binaryPath, shader);
		return shader;
	}
	void ShaderLibrary::DumpBinary(const std::filesystem::path& dest, Shader& shader)
	{
		std::vector<char> charVec = { shader.spvAssembly.cbegin(), shader.spvAssembly.cend() };
		FilesystemUtils::WriteBinary(dest, charVec);
	}
	std::vector<char> ShaderLibrary::ReadBinary(const std::filesystem::path& binPath)
	{
		std::vector<char> charData;
		FilesystemUtils::ReadBinary(binPath, charData);
		return charData;
	}
}