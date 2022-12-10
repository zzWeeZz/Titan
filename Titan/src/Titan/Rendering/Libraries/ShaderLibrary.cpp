#include "TNpch.h"
#include "ShaderLibrary.h"
#include "shaderc/shaderc.hpp"
#include "SPIRV-Reflect/spirv_reflect.h"
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
		else if (extension == ".mesh")
		{
			shader.shaderType = ShaderType::Mesh;
		}
		else if (extension == ".Task")
		{
			shader.shaderType = ShaderType::Task;
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
			Reflect(shader);
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

		Reflect(shader);

		DumpBinary(binaryPath, shader);
		return shader;
	}

	void ShaderLibrary::DumpBinary(const std::filesystem::path& dest, Shader& shader)
	{
		std::vector<char> charVec = { shader.spvAssembly.cbegin(), shader.spvAssembly.cend() };
		FilesystemUtils::WriteBinary(dest, charVec);
	}

	void ShaderLibrary::Reflect(Shader& shader)
	{
		SpvReflectShaderModule spvModule;
		auto reflectResult = spvReflectCreateShaderModule(shader.spvBinary.size() * 4, reinterpret_cast<void*>(shader.spvBinary.data()), &spvModule);
		TN_CORE_ASSERT(reflectResult == SPV_REFLECT_RESULT_SUCCESS, "Could not reflect shader.");

		shader.stage = GetShaderStage(spvModule);

		if (spvModule.push_constant_block_count > 0 && spvModule.push_constant_blocks != nullptr)
		{
			shader.pushConstants = { .stageFlags = shader.stage,.offset = spvModule.push_constant_blocks->offset, .size = spvModule.push_constant_blocks->size };
		}

		uint32_t spvBindingCount = 0;

		spvReflectEnumerateDescriptorBindings(&spvModule, &spvBindingCount, nullptr);

		std::vector<SpvReflectDescriptorBinding*> spvBindings(spvBindingCount);
		spvReflectEnumerateDescriptorBindings(&spvModule, &spvBindingCount, spvBindings.data());

		shader.layouts.reserve(spvBindingCount);
		for (uint32_t layoutBindingIndex = 0; layoutBindingIndex < spvBindingCount; ++layoutBindingIndex)
		{
			VkDescriptorType descType{};
			switch (spvBindings[layoutBindingIndex]->descriptor_type)
			{
			case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
				descType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				break;
			case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
				descType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				break;

			case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:
				descType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
				break;
			case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
				descType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				break;
			default:
				break;
			}
			VkDescriptorSetLayoutBinding descriptorSetLayoutBinding{};
			descriptorSetLayoutBinding.binding = spvBindings[layoutBindingIndex]->binding;
			descriptorSetLayoutBinding.descriptorCount = 1;
			descriptorSetLayoutBinding.descriptorType = descType;
			descriptorSetLayoutBinding.stageFlags = shader.stage;
			descriptorSetLayoutBinding.pImmutableSamplers = nullptr;

			shader.layouts[spvBindings[layoutBindingIndex]->set].push_back(descriptorSetLayoutBinding);
		}
		spvReflectDestroyShaderModule(&spvModule);
	}

	std::vector<char> ShaderLibrary::ReadBinary(const std::filesystem::path& binPath)
	{
		std::vector<char> charData;
		FilesystemUtils::ReadBinary(binPath, charData);
		return charData;
	}
	VkShaderStageFlags ShaderLibrary::GetShaderStage(SpvReflectShaderModule& spvModule)
	{
		switch (spvModule.shader_stage)
		{
		case SPV_REFLECT_SHADER_STAGE_COMPUTE_BIT:
			return VK_SHADER_STAGE_COMPUTE_BIT;

		case SPV_REFLECT_SHADER_STAGE_TASK_BIT_NV:
			return VK_SHADER_STAGE_TASK_BIT_NV;

		case SPV_REFLECT_SHADER_STAGE_MESH_BIT_NV:
			return VK_SHADER_STAGE_MESH_BIT_NV;

		case SPV_REFLECT_SHADER_STAGE_VERTEX_BIT:
			return VK_SHADER_STAGE_VERTEX_BIT;

		case SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT:
			return VK_SHADER_STAGE_FRAGMENT_BIT;

		default:
			TN_CORE_ASSERT(false, "Unsupported SpvReflectShaderStageFlagBits!");
			return VK_SHADER_STAGE_COMPUTE_BIT;
		}
	}
}